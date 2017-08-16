#include <stdio.h>
#include "contiki.h"
#include "dev/dht22.h"
/*---------------------------------------------------------------------------*/
PROCESS(dht22_process, "DHT22 test");
AUTOSTART_PROCESSES(&dht22_process);
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dht22_process, ev, data)
{
  int temperature, humidity;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(dht22);

  /* Let it spin and read sensor data */

  while(1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /* The standard sensor API may be used to read sensors individually, using
     * the `dht22.value(DHT22_READ_TEMP)` and `dht22.value(DHT22_READ_HUM)`,
     * however a single read operation (5ms) returns both values, so by using
     * the function below we save one extra operation
     */
    if(dht22_read_all(&temperature, &humidity) != DHT22_ERROR) {
      printf("Temperature %02d.%02d ºC, ", temperature / 10, temperature % 10);
      printf("Humidity %02d.%02d RH\n", humidity / 10, humidity % 10);
    } else {
      printf("Failed to read the sensor\n");
    }
  }
  PROCESS_END();
}
