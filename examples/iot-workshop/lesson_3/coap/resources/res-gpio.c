#include "contiki.h"
#include "rest-engine.h"
#include "dev/gpio.h"

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_gpio,
         "title=\"GPIO PIN PA5\";rt=\"Control\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  if(GPIO_READ_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK)) {
    GPIO_CLR_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
  } else {
    GPIO_SET_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
  }
}
