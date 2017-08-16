#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "lib/sensors.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
/*---------------------------------------------------------------------------*/
PROCESS(adc_process, "ADC process");
AUTOSTART_PROCESSES(&adc_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(adc_process, ev, data)
{
  PROCESS_BEGIN();

  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);

  /* Connect an analogue sensor and measure its value! */

  etimer_set(&et, CLOCK_SECOND);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    leds_toggle(LEDS_GREEN);

    printf("ADC1 = %u mV\n", adc_zoul.value(ZOUL_SENSORS_ADC1));
    printf("ADC3 = %u mV\n", adc_zoul.value(ZOUL_SENSORS_ADC3));

    etimer_reset(&et);
  }

  PROCESS_END();
}

