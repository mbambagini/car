//clear; g++ -lncurses -lpthread test.cpp -o test

/*
 keys:
 -  q: toggle the right on the left
 -  w: toggle the light in the center
 -  e: toggle the light on the right
 -  s: toggle the breaking
 -  \: engine power at   0%
 -  1: engine power at  10%
 -  2: engine power at  20%
 -  3: engine power at  30%
 -  4: engine power at  40%
 -  5: engine power at  50%
 -  6: engine power at  60%
 -  7: engine power at  70%
 -  8: engine power at  80%
 -  9: engine power at  90%
 - 10: engine power at 100%
 - KEY_UP:    forward
 - KEY_DOWN:  backward
 - KEY_LEFT:  steering left
 - KEY_RIGHT: steering right
 - A: echo ECM
 - B: echo BCM
 - t: update time
*/

#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h> 


WINDOW* init_window ();
void close_window ();

int row, col;

pthread_t thread_input;
pthread_t thread_screen;
pthread_t thread_comm;

// IO port descriptor
int fd;

// this struct and object contains the actual state of the car
struct car_t {
  bool hit_front;
  bool hit_left;
  bool hit_right;
  bool hit_rear;
  char speed;
  bool forward;
  char steering;
  bool breaking;
  char echo_bcm;
  char echo_ecm;
  unsigned char eye_l;
  unsigned char eye_r;
  bool light_r;
  bool light_c;
  bool light_l;
  bool light_sens;
} car;

// mutex to access car
pthread_mutex_t car_mutex;


/*****************************************************************************
 * Thread declaration
 *****************************************************************************/

// this function is the thread which takes the input from the UI
void* function_input_reader (void* p);

// this function shows car state on the screen
void* function_output_screen (void* p);

// this function handles the communication with the gateway and updates the
// car state
void* function_comm (void* p);

/*****************************************************************************
 * Init and closing function declaration
 *****************************************************************************/

// initialize comm device
bool init_hw (const char* filename);

// init UI
WINDOW* init_window ();

// close UI
void close_window();


/*****************************************************************************
 * Main function
 * the first argument must be the device that communicates with the gateway
 *****************************************************************************/

int main(int argc, char* argv[]) {
  WINDOW *w;
  
  if (argc == 1)
    return -1;  

  memset(&car, 0, sizeof(car_t));
  w = init_window();

  init_hw(argv[1]);

  pthread_create(&thread_input,  NULL, function_input_reader, (void*)w);
  pthread_create(&thread_screen, NULL, function_output_screen, (void*)w);
  pthread_create(&thread_comm,     NULL, function_comm, NULL);

  pthread_join(thread_input, NULL);
  pthread_join(thread_screen, NULL);
  pthread_join(thread_comm, NULL);

  close_window();

  return 0;
}


/*****************************************************************************
 * Communication callbacks and data
 *****************************************************************************/

typedef void (*data_handler_t)(char *b);

struct cmd_t {
  char code;
  char n_rx_bytes;
  char n_tx_bytes;
  data_handler_t tx_f;
  data_handler_t rx_f;
  cmd_t(char c, char n_rx, char n_tx, data_handler_t rx_fun,
        data_handler_t tx_fun) : code(c), n_rx_bytes(n_rx), n_tx_bytes(n_tx),
                                 rx_f(rx_fun), tx_f(tx_fun) {}
};

unsigned char compute_filter(int *id, unsigned char *b, unsigned char val) {
  unsigned char ret;

  b[(*id) % 10] = val & 0x7f;
  *id = *id + 1;

  double sum = b[0];
  for (int i = 1; i < 10; i++)
      sum += b[i];
   sum = sum / 10;
   ret = (unsigned char)sum;
}

void callback_eye_right_bcm (char *b) {
  static unsigned char arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int id = 0;

  unsigned char val = compute_filter(&id, arr, b[0]);

  pthread_mutex_lock(&car_mutex);
  car.eye_r = val;
  pthread_mutex_unlock(&car_mutex);
}

void callback_eye_left_bcm (char *b) {
  static unsigned char arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int id = 0;

  unsigned char val = compute_filter(&id, arr, b[0]);

  pthread_mutex_lock(&car_mutex);
  car.eye_l = val;
  pthread_mutex_unlock(&car_mutex);
}

void callback_command_bcm (char *b) {
  pthread_mutex_lock(&car_mutex);
  b[0] = 0x80;
  if (car.breaking)
    b[0] |=  0x01;
  if (car.light_r)
    b[0] |=  0x02;
  if (car.light_c)
    b[0] |=  0x04;
  if (car.light_l)
    b[0] |=  0x08;
  pthread_mutex_unlock(&car_mutex);
}

void callback_status_bcm (char *b) {
  pthread_mutex_lock(&car_mutex);
  car.hit_front =   b[0] & 0x01;
  car.hit_left =   (b[0] & 0x02) ? 1 : 0;
  car.hit_right =  (b[0] & 0x04) ? 1 : 0;
  car.hit_rear =   (b[0] & 0x08) ? 1 : 0;
  car.light_sens = (b[0] & 0x10) ? 1 : 0;
  pthread_mutex_unlock(&car_mutex);
}

void callback_steering_ecm (char *b) {
  char v;
  pthread_mutex_lock(&car_mutex);
  v = car.steering;
  pthread_mutex_unlock(&car_mutex);
  b[0] = v==10 ? 11 : v;
}
void callback_engine_ecm (char *b) {
  char v;
  pthread_mutex_lock(&car_mutex);
  v = car.speed * car.forward;
  pthread_mutex_unlock(&car_mutex);
  b[0] = v==10 ? 11 : v;
}

cmd_t cmds[] = { //BCM:
                cmd_t(0x10, 1, 0, callback_status_bcm, NULL),    //status
                cmd_t(0x11, 1, 0, callback_eye_right_bcm, NULL), //right eye
                cmd_t(0x12, 1, 0, callback_eye_left_bcm, NULL),  //left eye
                cmd_t(0x13, 0, 1, NULL, callback_command_bcm),   //commands
                //ECM:
                cmd_t(0x20, 0, 1, callback_steering_ecm, NULL), //steering
                cmd_t(0x21, 0, 1, callback_engine_ecm, NULL),   //engine
                //DIAG:
                //cmd_t(0x30, 0, 1, callback_echo_bcm, NULL), //echo bcm
                //cmd_t(0x31, 0, 1, callback_echo_ecm, NULL), //echo ecm
                //cmd_t(0x32, 1, 0, NULL, callback_diag_res), //diag response
               };


/*****************************************************************************
 * Initialization and termination functions
 *****************************************************************************/

bool init_hw (const char* filename) {
  fd = open(filename, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(fd == -1)
    return false;
  return true;
}

WINDOW* init_window () {
  WINDOW* w;

  initscr();     // Start curses mode
  clear();       // Clear windows
  cbreak();      // No buffer data but handle signals - raw otherwise
  noecho();      // No character echo
  start_color(); // Start colors
  init_pair(1, COLOR_RED, COLOR_WHITE);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  getmaxyx(stdscr, row, col);
  w = newwin(col, row, 0, 0);
  keypad(w, TRUE);  // Enable Fx and keys
  refresh();     // Clear windows

  return w;
}

void close_window() {
  clrtoeol();
  refresh();
  endwin();
}


/*****************************************************************************
 * Thread functions
 *****************************************************************************/

void* function_input_reader (void* p) {
  WINDOW* w = (WINDOW*)p;
  while(1) {
    int c = wgetch(w);
    pthread_mutex_lock(&car_mutex);
    switch(c) {
       case '\\': car.speed =  0; break;
       case '1': car.speed =  10; break;
       case '2': car.speed =  20; break;
       case '3': car.speed =  30; break;
       case '4': car.speed =  40; break;
       case '5': car.speed =  50; break;
       case '6': car.speed =  60; break;
       case '7': car.speed =  70; break;
       case '8': car.speed =  80; break;
       case '9': car.speed =  90; break;
       case '0': car.speed = 100; break;
       case KEY_UP:   car.forward = 1; break;
       case KEY_DOWN: car.forward = 0; break;
       case KEY_RIGHT:
           if (car.steering <= 99)
               car.steering++;
           break;
       case KEY_LEFT:
           if (car.steering >= -99)
               car.steering--;
           break;
       case 's':   car.breaking = !car.breaking; break;
       case 'q':   car.light_l = !car.light_l; break;
       case 'w':   car.light_c = !car.light_c; break;
       case 'e':   car.light_r = !car.light_r; break;
    };
    pthread_mutex_unlock(&car_mutex);
  }
  return NULL;
}

void* function_output_screen (void* p) {
  WINDOW* w = (WINDOW*)p;
  unsigned long int i = 0;

  while(1) {
      int row = 1;

      pthread_mutex_lock(&car_mutex);
      car_t car_ = car;
      pthread_mutex_unlock(&car_mutex);

      clear();
      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "REMOTE CONTROLLER CAR - PC side");
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "Refresh counter: %ld", i);
      mvprintw(row++, 2, "Refresh rate: %fs", 0.1);
      row++;

      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "ECM", i);
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "Speed: %d", car_.forward ? car_.speed : -car_.speed);
      mvprintw(row++, 2, "Steering: %d", car_.steering);
      if (car_.breaking)
          attron(COLOR_PAIR(1));
      mvprintw(row++, 2, "Breaking: %d", car_.breaking);
      if (car_.breaking)
          attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
      mvprintw(row++, 2, "Light sensor: %s", 
                                           car_.light_sens? "bright" : "dark");
      attroff(COLOR_PAIR(2));
      row++;

      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "BCM");
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "Lights (left, center, right): %d %d %d", 
                                     car_.light_l, car_.light_c, car_.light_r);
      mvprintw(row++, 2, "Hit status (front, left, right, rear): %d %d %d %d",
                 car_.hit_front, car_.hit_left, car_.hit_right, car_.hit_rear);
      mvprintw(row++, 2, "Eyes (left, right): %d %d", car_.eye_l, car_.eye_r);
      row++;

      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "DIAG");
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "BCM (%s), ECM (%s)", "off", "off");
      row++;

      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "TIME");
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "%s", "off");
      row++;

      i++;
      refresh();
      usleep(100000); // 0.1 second
  }
  return NULL;
}

void* function_comm (void* p) {
  char buffer_tx[10];
  char buffer_rx[10];

  while(1) {
    for (int i = 0; i < sizeof(cmds)/sizeof(cmd_t); i++) {
      unsigned long int iters = 0;
      // send command
      buffer_tx[0] = cmds[i].code;
      // collect additional data
      if (cmds[i].tx_f != NULL && cmds[i].n_tx_bytes > 0)
          cmds[i].tx_f(&buffer_tx[1]);
      // send data
      write(fd, buffer_tx, 1+cmds[i].n_tx_bytes);
      // receive response
      if (cmds[i].n_rx_bytes > 0) {
        int tot = 0;
        while (true) {
          iters++;
          if (iters > 10000)
             break;
          int num = read(fd, &buffer_rx[tot], cmds[i].n_rx_bytes+1-tot);
          if (num == -1)
            continue;
          tot += num;
          if (tot == (cmds[i].n_rx_bytes+1))
            break;
        }
        // use response
        if (buffer_rx[cmds[i].n_rx_bytes] == '\n' && cmds[i].rx_f != NULL)
          cmds[i].rx_f(buffer_rx);
      }
      usleep(1000); // wait 1 ms between two consecutive tx
    }
    usleep(100000); // wait 100 ms between two consecutive burst
  }

  return 0;
}

