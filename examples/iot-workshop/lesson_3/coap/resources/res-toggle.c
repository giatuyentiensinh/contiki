#include "contiki.h"
#include "rest-engine.h"
#include "dev/leds.h"

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_toggle,
         "title=\"Red LED\";rt=\"Control\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	leds_toggle(LEDS_RED);
}
