#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/adc-zoul.h"
#include "dev/dht22.h"
/*---------------------------------------------------------------------------*/
extern resource_t res_exercise3;
/*---------------------------------------------------------------------------*/
PROCESS(exercise3, "Exercise3 process");
AUTOSTART_PROCESSES(&exercise3);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(exercise3, ev, data)
{
  PROCESS_BEGIN();

  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);
  SENSORS_ACTIVATE(dht22);
  rest_init_engine();
  rest_activate_resource(&res_exercise3, "iot/contiki-remote");
  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
