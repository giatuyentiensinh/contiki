#include "contiki.h"
#include <string.h>
#include "rest-engine.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_zoul,
         "title=\"Zoul on-board sensors\";rt=\"zoul\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  uint16_t temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED); 
  uint16_t adc1 = adc_zoul.value(ZOUL_SENSORS_ADC1);
  uint16_t adc2 = adc_zoul.value(ZOUL_SENSORS_ADC2);
  uint16_t adc3 = adc_zoul.value(ZOUL_SENSORS_ADC3);

  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "Temp %d.%u; ADC1 %u ADC2 %u ADC3 %u",
             temp / 1000, temp % 1000, adc1, adc2, adc3);
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));

  } else if(accept == REST.type.APPLICATION_XML) {
    REST.set_header_content_type(response, REST.type.APPLICATION_XML);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE,
             "<Temp>%d.%u</Temp><ADC1>%u</ADC1><ADC2>%u</ADC2><ADC3>%u</ADC3>",
             temp / 1000, temp % 1000, adc1, adc2, adc3);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else if(accept == REST.type.APPLICATION_JSON) {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'Temp':%d.%u,'ADC1':%u,'ADC2':%u,'ADC3':%u}",
             temp / 1000, temp % 1000, adc1, adc2, adc3);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}