#include "contiki.h"
#include "cpu.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/motion-sensor.h"

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define LEDS_OFF_HYSTERISIS      RTIMER_SECOND
/*---------------------------------------------------------------------------*/
static struct rtimer rt;
/*---------------------------------------------------------------------------*/
PROCESS(presence_sensor, "Motion sensor process");
AUTOSTART_PROCESSES(&presence_sensor);
/*---------------------------------------------------------------------------*/
void
rt_callback(struct rtimer *t, void *ptr)
{
  leds_off(LEDS_RED);
}
/*---------------------------------------------------------------------------*/
static void
presence_callback(uint8_t value)
{
  printf("*** Presence detected!\n");
  leds_on(LEDS_RED);
  rtimer_set(&rt, RTIMER_NOW() + LEDS_OFF_HYSTERISIS, 1, rt_callback, NULL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(presence_sensor, ev, data)
{
  PROCESS_BEGIN();

  /* Register the callback handler when presence is detected */
  MOTION_REGISTER_INT(presence_callback);

  /* Enable the sensor, as default it assumes the signal pin has a pull-down
   * resistor and a rising interrupt (signal goes high when motion is detected),
   * this is the case of the Grove's PIR sensor v.1.0.  If the sensor has an
   * inverse logic, change at the motion-sensor.c driver file
   * GPIO_DETECT_FALLING instead of GPIO_DETECT_RISING if using an external
   * pull-up resistor
   */
  SENSORS_ACTIVATE(motion_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
