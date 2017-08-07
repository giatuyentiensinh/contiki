#include "contiki.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "dev/zoul-sensors.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

#define MAX_AGE      60
#define INTERVAL_MIN 10
#define INTERVAL_MAX (MAX_AGE - 1)
#define CHANGE       1000

static int32_t interval_counter = INTERVAL_MIN;
static int temperature_old = INT_MIN;

PERIODIC_RESOURCE(res_tracking_temp,
         "title=\"Tracking Temperature\";obs",
         res_get_handler,
         NULL,
         NULL,
         NULL,
         CLOCK_SECOND,
         res_periodic_handler);

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	int temperature = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

	unsigned int accept = -1;
	REST.get_header_accept(request, &accept);

	if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", temperature);

		REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
	} else if(accept == REST.type.APPLICATION_JSON) {
		REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'temperature':%d}", temperature);

		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	} else {
		REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
		const char *msg = "Supporting content-types text/plain and application/json";
		REST.set_response_payload(response, msg, strlen(msg));
	}
	REST.set_header_max_age(response, MAX_AGE);
}

static void res_periodic_handler() {
	int temperature = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
	++interval_counter;
	if((abs(temperature - temperature_old) >= CHANGE &&
			interval_counter >= INTERVAL_MIN) ||
			interval_counter >= INTERVAL_MAX) {
		interval_counter = 0;
		temperature_old = temperature;
		REST.notify_subscribers(&res_tracking_temp);
	}
}
