#include "contiki.h"

#include <string.h>
#include "rest-engine.h"
#include "dev/leds.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_leds,
         "title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"",
         NULL,
         res_post_put_handler,
         res_post_put_handler,
         NULL);

static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	size_t len = 0;
	const char *color = NULL;
	const char *mode = NULL;
	uint8_t led = 0;
	int success = 1;

	if((len = REST.get_query_variable(request, "color", &color))) {
		PRINTF("color %.*s\n", len, color);
		if(strncmp(color, "r", len) == 0)
			led = LEDS_RED;
		else if(strncmp(color, "g", len) == 0)
			led = LEDS_GREEN;
		else if(strncmp(color, "b", len) == 0)
			led = LEDS_BLUE;
		else
			success = 0;
	} else
		success = 0;

	if(success && (len = REST.get_post_variable(request, "mode", &mode))) {
		PRINTF("mode %s\n", mode);
		if(strncmp(mode, "on", len) == 0)
			leds_on(led);
		else if(strncmp(mode, "off", len) == 0)
			leds_off(led);
		else
			success = 0;
	} else
		success = 0;

	if(!success)
		REST.set_response_status(response, REST.status.BAD_REQUEST);
}
