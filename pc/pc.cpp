//clear; g++ -lncurses -lpthread test.cpp -o test

//char str[80];
//getstr(str);

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

 message:
 - byte 0:  presence byte
   - bit 0: steering byte presence
   - bit 1: engine power byte presence
   - bit 2: echo ECM byte presence
   - bit 3: echo BCM byte presence
   - bit 4: command byte presence
   - bit 5: 0
   - bit 6: 0
   - bit 7: 0
 - byte 1: steering byte [-100: +100]
 - byte 2: engine power byte [-100: +100]
 - byte 3: echo ECM byte
 - byte 4: echo BCM byte
 - byte 5: command byte
   - bit 0: light right command
   - bit 1: light center command
   - bit 2: light left command
   - bit 3: break command
   - bit 4: 0
   - bit 5: 0
   - bit 6: 0
   - bit 7: 0
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

int fd; // file description for the serial port

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

  bool light_r;
  bool light_c;
  bool light_l;

  bool light_sens;   
} car;


pthread_mutex_t car_mutex;


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
/*
       case KEY_F(4):  mesg = "F4"; break;
       case KEY_F(5):  mesg = "F5"; break;
       case KEY_F(6):  mesg = "F6"; break;
       case 'a':       attron(A_BOLD | A_DIM | A_UNDERLINE | A_REVERSE);  break;
       case 's':       attroff(A_BOLD | A_DIM | A_UNDERLINE | A_REVERSE); break;
*/
       default:;
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
      car_t car_local = car;
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
      mvprintw(row++, 2, "Speed: %d", car_local.forward ? car_local.speed :
                                                             -car_local.speed);
      mvprintw(row++, 2, "Steering: %d", car_local.steering);
      if (car_local.breaking)
          attron(COLOR_PAIR(1));
      mvprintw(row++, 2, "Breaking: %d", car_local.breaking);
      if (car_local.breaking)
          attroff(COLOR_PAIR(1));
      attron(COLOR_PAIR(2));
      mvprintw(row++, 2, "Light sensor: %s", 
                                      car_local.light_sens? "bright" : "dark");
      attroff(COLOR_PAIR(2));
      row++;

      attron(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "BCM");
      attroff(A_BOLD | A_UNDERLINE);
      mvprintw(row++, 2, "Lights (left, center, right): %d %d %d",
                      car_local.light_l, car_local.light_c, car_local.light_r);
      mvprintw(row++, 2, "Hit status (front, left, right, rear): %d %d %d %d",
                                      car_local.hit_front, car_local.hit_left,
                                      car_local.hit_right, car_local.hit_rear);
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

      i++;
      refresh();
      usleep(100000);
  }
  return NULL;
}



void* function_comm (void* p) {

  while(1) {

    pthread_mutex_lock(&car_mutex);
    car_t car_local = car;
    pthread_mutex_unlock(&car_mutex);

    int count = 1;

    char array[6] = {0, 0, 0, 0, 0, 0};
    // steering byte
    array[0] = array[0] | 0x01;
    array[1] = car_local.steering;
    count++;
    // engine power byte
    array[0] = array[0] | 0x02;
    array[2] = car_local.speed;
    if (car_local.forward == false)
      array[2] = -array[2];
    count++;
    // echo ECM byte
    array[0] = array[0] | 0x04;
    // echo BCM byte
    array[0] = array[0] | 0x08;
    // command byte
    array[0] = array[0] | 0x10;
    array[5] = array[5] |  car_local.light_r;
    array[5] = array[5] | (car_local.light_c << 1);
    array[5] = array[5] | (car_local.light_l << 2);
    array[5] = array[5] | (car_local.breaking << 3);
    count++;

    write(fd, array, count);  //Send data
    usleep(1000000); //1 sec
  }

  return 0;
}

bool init_hw () {
  fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	
  if(fd == -1) {
    //perror("open_port: Unable to open /dev/ttyS0 - ");
    printf("open_port: Unable to open /dev/ttyS0. \n");
  } else {
    fcntl(fd, F_SETFL, 0);
    printf("port is open.\n");
  }

  struct termios port_settings;      // structure to store the port settings in

  cfsetispeed(&port_settings, B115200);    // set baud rates
  cfsetospeed(&port_settings, B115200);

  port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;
	
  tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port
}

int main() {
  WINDOW *w;

  char *mesg;

  memset(&car, 0, sizeof(car_t));
  w = init_window();

  pthread_create(&thread_input, NULL, function_input_reader, (void*)w);
  pthread_create(&thread_screen, NULL, function_output_screen, (void*)w);
  pthread_create(&thread_comm, NULL, function_comm, NULL);

  pthread_join(thread_input, NULL);
  pthread_join(thread_screen, NULL);
  pthread_join(thread_comm, NULL);

  close_window();

  return 0;
}


WINDOW* init_window () {
  WINDOW* w;

  initscr();     // Start curses mode
  clear();       // Clear windows
  cbreak();      // No buffer data but handle signals - raw otherwise
  noecho();      // No character echo
  start_color(); // Start colors

/*
  COLOR_BLACK   0
  COLOR_RED     1
  COLOR_GREEN   2
  COLOR_YELLOW  3
  COLOR_BLUE    4
  COLOR_MAGENTA 5
  COLOR_CYAN    6
  COLOR_WHITE   7
  init_color(COLOR_RED, 700, 0, 0);
    // param 1     : color name
    // param 2, 3, 4 : rgb content min = 0, max = 1000
*/
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

