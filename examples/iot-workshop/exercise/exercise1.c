#include "contiki.h"
#include "sys/etimer.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/gpio.h"

#define PORT_BASE  GPIO_PORT_TO_BASE(GPIO_A_NUM)
#define PIN_MASK   GPIO_PIN_MASK(4)
/*---------------------------------------------------------------------------*/
PROCESS(exercise1, "exercise1 process");
AUTOSTART_PROCESSES(&exercise1);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(exercise1, ev, data)
{
  static struct etimer periodic;
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);

  GPIO_SOFTWARE_CONTROL(PORT_BASE, PIN_MASK);
  GPIO_SET_OUTPUT(PORT_BASE, PIN_MASK);
  GPIO_SET_PIN(PORT_BASE, PIN_MASK);

  etimer_set(&periodic, 10 * CLOCK_SECOND);
  while (1) {
	PROCESS_YIELD();

	if(ev == sensors_event && data == &button_sensor) {
		if(GPIO_READ_PIN(PORT_BASE, PIN_MASK)) {
		  GPIO_CLR_PIN(PORT_BASE, PIN_MASK);
		} else {
		  GPIO_SET_PIN(PORT_BASE, PIN_MASK);
		}
	}

	if(etimer_expired(&periodic)) {
	  leds_toggle(LEDS_RED);
	  etimer_reset(&periodic);
	}
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
