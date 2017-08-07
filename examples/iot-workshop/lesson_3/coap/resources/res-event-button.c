#include "rest-engine.h"
#include "er-coap.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

EVENT_RESOURCE(res_event_button,
               "title=\"Event button\";obs",
               res_get_handler,
               NULL,
               NULL,
               NULL,
               res_event_handler);

static uint16_t event_counter = 0;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Button press count: %u", event_counter));
}

static void
res_event_handler(void) {
	++event_counter;
	if(1) {
		printf("TICK %u for /%s\n", event_counter, res_event_button.url);
		REST.notify_subscribers(&res_event_button);
	}
}
