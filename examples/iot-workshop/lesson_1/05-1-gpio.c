#include "contiki.h"
#include <stdio.h>
#include "dev/leds.h"
#include "sys/etimer.h"
#include "dev/gpio.h"

/* Pin operations are done in a mask fashion, so if you want to drive Pin 1
 * then this corresponds to (1<<N), with an actual value of 2.  If the Pin was
 * the number 2 then the mask value would be 4
 */

/* The masks below converts the Port number and Pin number to base and mask values */
#define EXAMPLE_PORT_BASE  GPIO_PORT_TO_BASE(GPIO_A_NUM)
#define EXAMPLE_PIN_MASK   GPIO_PIN_MASK(5)
/*---------------------------------------------------------------------------*/
PROCESS(gpio_process, "GPIO process");
AUTOSTART_PROCESSES(&gpio_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gpio_process, ev, data)
{
  PROCESS_BEGIN();

  /* We tell the system the application will drive the pin */
  GPIO_SOFTWARE_CONTROL(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

  /* And set as output, starting low */
  GPIO_SET_OUTPUT(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
  GPIO_SET_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

  /* Spin the timer */
  etimer_set(&et, CLOCK_SECOND * 5);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    if(GPIO_READ_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK)) {
      GPIO_CLR_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
    } else {
      GPIO_SET_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
    }
    leds_toggle(LEDS_GREEN);
    etimer_reset(&et);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
