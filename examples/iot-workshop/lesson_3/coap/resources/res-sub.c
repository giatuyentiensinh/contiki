#include <string.h>
#include "rest-engine.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

PARENT_RESOURCE(res_sub,
                "title=\"Sub-resource demo\"",
                res_get_handler,
                NULL,
                NULL,
                NULL);

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	const char *uri_path = NULL;

	int len = REST.get_url(request, &uri_path);
	int base_len = strlen(res_sub.url);

	if(len == base_len)
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "Request any sub-resource of /%s", res_sub.url);
	else
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, ".%.*s", len - base_len, uri_path + base_len);
	REST.set_response_payload(response, buffer, strlen((char *)buffer));
}
