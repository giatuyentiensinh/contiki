#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/adc-zoul.h"
#include "dev/button-sensor.h"
#include "dev/dht22.h"
#include "dev/gpio.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_leds,
  res_toggle,
  res_event_button,
  res_sub,
  res_zoul,
  res_dht22,
  res_gpio,
  res_tracking_temp;

PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();
  PRINTF("CoAP Erbium Example Server\n");

  /* ADC zoul configure */
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL);

  /* Enable sensor dht22 */
  SENSORS_ACTIVATE(dht22);

  /* GPIO PIN configure */
  GPIO_SOFTWARE_CONTROL(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

    /* And set as output, starting low */
  GPIO_SET_OUTPUT(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
  GPIO_SET_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

  /* Initialize the REST engine. */
  rest_init_engine();

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
  rest_activate_resource(&res_hello, "test/hello");
  rest_activate_resource(&res_leds, "actuators/leds");
  rest_activate_resource(&res_toggle, "actuators/toggle");
  rest_activate_resource(&res_event_button, "actuators/event_button");
  rest_activate_resource(&res_sub, "test/sub");
  rest_activate_resource(&res_zoul, "sensors/zoul");
  rest_activate_resource(&res_dht22, "sensors/dht22");
  rest_activate_resource(&res_gpio, "sensors/gpio");
  rest_activate_resource(&res_tracking_temp, "sensors/tempOnBoard");

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == sensors_event && data == &button_sensor) {
      PRINTF("*******BUTTON*******\n");
      /* Call the event_handler for this application-specific event. */
      res_event_button.trigger();
    }
  }

  PROCESS_END();
}
