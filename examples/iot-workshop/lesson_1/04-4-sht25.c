#include <stdio.h>
#include "contiki.h"
#include "dev/sht25.h"
/*---------------------------------------------------------------------------*/
PROCESS(sht25_process, "SHT25 process");
AUTOSTART_PROCESSES(&sht25_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sht25_process, ev, data)
{
  int16_t temperature, humidity;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(sht25);

  /* Check if the sensor voltage operation is over 2.25V */
  if(sht25.value(SHT25_VOLTAGE_ALARM)) {
    printf("Voltage is lower than recommended for the sensor operation\n");
    PROCESS_EXIT();
  }

  /* Configure the sensor for maximum resolution (14-bit temperature, 12-bit
   * relative humidity), this will require up to 85ms for the temperature
   * integration, and 29ms for the relative humidity (this is the default
   * setting at power on).  To achieve a faster integration time at the cost
   * of a lower resolution, change the value below accordingly, see sht25.h.
   */
  sht25.configure(SHT25_RESOLUTION, SHT2X_RES_14T_12RH);

  /* Let it spin and read sensor data */

  while(1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    temperature = sht25.value(SHT25_VAL_TEMP);
    printf("Temperature %02d.%02d ºC, ", temperature / 100, temperature % 100);
    humidity = sht25.value(SHT25_VAL_HUM);
    printf("Humidity %02d.%02d RH\n", humidity / 100, humidity % 100);
  }
  PROCESS_END();
}