#include "contiki.h"
#include <stdio.h>
#include "sys/etimer.h"

/* These library allows to use the on-board sensors */
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(onboard_sensors_process, "On-board sensors process");
AUTOSTART_PROCESSES(&onboard_sensors_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(onboard_sensors_process, ev, data)
{
  PROCESS_BEGIN();

  static uint16_t temp;
  static uint32_t batt;

  /* Spin the timer */
  etimer_set(&et, CLOCK_SECOND);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /* Read the sensors */
    batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
    printf("Batt = %u mV\n", (uint16_t)batt);

    temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
    printf("Core temperature = %d.%u C\n", temp / 1000, temp % 1000);

    etimer_reset(&et);
  }

  PROCESS_END();
}
