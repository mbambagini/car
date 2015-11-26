#ifndef __BODY_HPP__
#define __BODY_HPP__

//BODY
#define HW_REAR_RIGHT_EYE            p19
#define HW_REAR_LEFT_EYE             p20
#define HW_FRONT_EYE_RX              p28
#define HW_FRONT_EYE_TX              p27
#define HW_HIT_FRONT                 p14
#define HW_HIT_REAR                  p15
#define HW_HIT_LEFT                  p16
#define HW_HIT_RIGHT                 p17
#define HW_FRONT_EYE_ADDR            0xE0
#define HW_FRONT_EYE_DEFAULT_ADDR    0xF2

/*! \brief initialize body entity (BCM)
 *
 */
void init_body();

/*! \brief body task (BCM)
 * this thread periodically:
 * - reads driver commands (such as lights)
 * - sends status to driver (such as collision detection and distances)
 *
 * \param args not used
 */
void thread_body (void const *args);

#endif //__BODY_HPP__

