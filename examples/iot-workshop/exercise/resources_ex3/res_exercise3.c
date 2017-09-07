#include "contiki.h"
#include "rest-engine.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_exercise3,
         "title=\"Exercise 3\";rt=\"zoul\"",
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
  uint16_t batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  int temperature, humidity;
  dht22_read_all(&temperature, &humidity);

  unsigned int accept = -1;
  REST.get_header_accept(request, &accept);

  if(accept == REST.type.APPLICATION_JSON) {
	  REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
	  snprintf((char *)buffer, REST_MAX_CHUNK_SIZE,  "{"
			  "'ADC1':%u,"
			  "'ADC2':%u,"
			  "'ADC3':%u,"
			  "'tempOnBoard':%u,"
			  "'battery':%u,"
			  "'DHT22_temp':%d,"
			  "'DHT22_humi':%d"
	  "}", adc1, adc2, adc3, temp, batt, temperature, humidity);
	  REST.set_response_payload(response, buffer, strlen((char *)buffer));
  } else {
	  REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
	  const char *msg = "Supporting content-types application/json";
	  REST.set_response_payload(response, msg, strlen(msg));
  }
}
