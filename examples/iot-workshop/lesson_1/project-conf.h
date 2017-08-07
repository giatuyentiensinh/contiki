/*---------------------------------------------------------------------------*/
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* Pin definition for the test-motion example, for the RE-Mote it uses the
 * ADC1 pin
 */
#define MOTION_SENSOR_PORT       GPIO_A_NUM
#define MOTION_SENSOR_PIN        5
#define MOTION_SENSOR_VECTOR     GPIO_A_IRQn

#endif /* PROJECT_CONF_H_ */