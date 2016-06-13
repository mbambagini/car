#ifndef __BODY_HPP__
#define __BODY_HPP__

//BODY
#define HW_RIGHT_EYE                 p19
#define HW_LEFT_EYE                  p20
#define HW_HIT_FRONT                 p12
#define HW_HIT_REAR                  p11
#define HW_HIT_LEFT                  p16
#define HW_HIT_RIGHT                 p15
#define HW_PHOTO                     p17
#define HW_LIGHT_L                   p27
#define HW_LIGHT_C                   p26
#define HW_LIGHT_R                   p25

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

