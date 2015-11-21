#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#define HW_STEERING_SERVO            p21
#define HW_ENGINE_ENABLER            p22
#define HW_ENGINE_DIR_1              p23
#define HW_ENGINE_DIR_2              p24
#define HW_ENGINE_PERIOD             (0.100)
#define HW_SERVO_RANGE_INIT          (0.0005)
#define HW_SERVO_ANGLE_INIT          (45.0)

/**
 * Initialization of the engine component:
 * - main engine
 * - steering motor
 */
void init_engine();

/**
 * The thread executes periodically:
 * - check inputs: if no message has arrived for 2.5 seconds, switch the
 *   actuators off
 * - actuation:
 *   - rotate steering servo motor
 *   - main engine:
 *     - if the driver is breaking, block the motor for no more than 1 seconds
 *       (while breaking the engine dissipates much power)
 *     - otherwise, set direction and set the current which flows into the 
 *       motor
 */
void thread_engine (void const *args);

#endif //__ENGINE_HPP__

