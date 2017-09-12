#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"

#include "tinydtls.h"

#include <string.h>
#include "dev/uart1.h"
#include "dev/serial-line.h"
#ifndef DEBUG
#define DEBUG DEBUG_PRINT
#endif
#include "net/ip/uip-debug.h"
#include "dtls_debug.h"
#include "dtls.h"

#ifdef ENABLE_POWERTRACE
#include "powertrace.h"
#endif
/*---------------------------------------------------------------------------*/
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])
#define MAX_PAYLOAD_LEN		120
/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_VIOLET	"\x1b[35m"
#define ANSI_COLOR_RESET 	"\x1b[0m"
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static dtls_context_t *dtls_context;
/*---------------------------------------------------------------------------*/
static const unsigned char ecdsa_priv_key[] = {
			0xD9, 0xE2, 0x70, 0x7A, 0x72, 0xDA, 0x6A, 0x05,
			0x04, 0x99, 0x5C, 0x86, 0xED, 0xDB, 0xE3, 0xEF,
			0xC7, 0xF1, 0xCD, 0x74, 0x83, 0x8F, 0x75, 0x70,
			0xC8, 0x07, 0x2D, 0x0A, 0x76, 0x26, 0x1B, 0xD4};

static const unsigned char ecdsa_pub_key_x[] = {
			0xD0, 0x55, 0xEE, 0x14, 0x08, 0x4D, 0x6E, 0x06,
			0x15, 0x59, 0x9D, 0xB5, 0x83, 0x91, 0x3E, 0x4A,
			0x3E, 0x45, 0x26, 0xA2, 0x70, 0x4D, 0x61, 0xF2,
			0x7A, 0x4C, 0xCF, 0xBA, 0x97, 0x58, 0xEF, 0x9A};

static const unsigned char ecdsa_pub_key_y[] = {
			0xB4, 0x18, 0xB6, 0x4A, 0xFE, 0x80, 0x30, 0xDA,
			0x1D, 0xDC, 0xF4, 0xF4, 0x2E, 0x2F, 0x26, 0x31,
			0xD0, 0x43, 0xB1, 0xFB, 0x03, 0xE2, 0x2F, 0x4D,
			0x17, 0xDE, 0x43, 0xF9, 0xF9, 0xAD, 0xEE, 0x70};
/*---------------------------------------------------------------------------*/
static int
read_from_peer(struct dtls_context_t *ctx, 
	       session_t *session, uint8 *data, size_t len) {
  size_t i;
  for (i = 0; i < len; i++)
    PRINTF("%c", data[i]);

  /* echo incoming application data */
  dtls_write(ctx, session, data, len);
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
send_to_peer(struct dtls_context_t *ctx, 
	     session_t *session, uint8 *data, size_t len) {

  struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

  uip_ipaddr_copy(&conn->ripaddr, &session->addr);
  conn->rport = session->port;

  PRINTF("send to ");
  PRINT6ADDR(&conn->ripaddr);
  PRINTF(":%u\n", uip_ntohs(conn->rport));

  uip_udp_packet_send(conn, data, len);

  /* Restore server connection to allow data from any node */
  memset(&conn->ripaddr, 0, sizeof(conn->ripaddr));
  memset(&conn->rport, 0, sizeof(conn->rport));

  return len;
}

#ifdef DTLS_PSK
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int
get_psk_info(struct dtls_context_t *ctx, const session_t *session,
	     dtls_credentials_type_t type,
	     const unsigned char *id, size_t id_len,
	     unsigned char *result, size_t result_length) {

	struct keymap_t {
		unsigned char *id;
		size_t id_length;
		unsigned char *key;
		size_t key_length;
	} psk[3] = { { (unsigned char *) "Client_identity", 15,
			(unsigned char *) "secretPSK", 9 }, {
			(unsigned char *) "default identity", 16,
			(unsigned char *) "\x11\x22\x33", 3 }, { (unsigned char *) "\0", 2,
			(unsigned char *) "", 1 } };

	if (type != DTLS_PSK_KEY) {
		return 0;
	}

	if (id) {
		int i;
		for (i = 0; i < sizeof(psk) / sizeof(struct keymap_t); i++) {
			if (id_len == psk[i].id_length
					&& memcmp(id, psk[i].id, id_len) == 0) {
				if (result_length < psk[i].key_length) {
					dtls_warn("buffer too small for PSK");
					return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
				}

				memcpy(result, psk[i].key, psk[i].key_length);
				return psk[i].key_length;
			}
		}
	}

	return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int
get_ecdsa_key(struct dtls_context_t *ctx,
	      const session_t *session,
	      const dtls_ecdsa_key_t **result) {
  static const dtls_ecdsa_key_t ecdsa_key = {
    .curve = DTLS_ECDH_CURVE_SECP256R1,
    .priv_key = ecdsa_priv_key,
    .pub_key_x = ecdsa_pub_key_x,
    .pub_key_y = ecdsa_pub_key_y
  };

  *result = &ecdsa_key;
  return 0;
}

static int
verify_ecdsa_key(struct dtls_context_t *ctx,
		 const session_t *session,
		 const unsigned char *other_pub_x,
		 const unsigned char *other_pub_y,
		 size_t key_size) {
  return 0;
}
#endif /* DTLS_ECC */

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
dtls_handle_read(dtls_context_t *ctx) {
	session_t session;

	if (uip_newdata()) {
		uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
		session.port = UIP_UDP_BUF->srcport;
		session.size = sizeof(session.addr) + sizeof(session.port);

		dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
	}
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
	int i;
	uint8_t state;
	PRINTF(ANSI_COLOR_RED);
	PRINTF("Server IPv6 addresses: \n");
	for (i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if (uip_ds6_if.addr_list[i].isused
				&& (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);PRINTF("\n");
		}
	} PRINTF(ANSI_COLOR_RESET);
}
/*---------------------------------------------------------------------------*/
void
init_dtls() {

	static dtls_handler_t cb = {
			.write = send_to_peer,
			.read  = read_from_peer,
			.event = NULL,
#ifdef DTLS_PSK
			.get_psk_info = get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
			.get_ecdsa_key = get_ecdsa_key,
			.verify_ecdsa_key = verify_ecdsa_key
#endif /* DTLS_ECC */
	};

	uip_ipaddr_t ipaddr;

	PRINTF("DTLS server started\n");

	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

	server_conn = udp_new(NULL, 0, NULL);
	udp_bind(server_conn, UIP_HTONS(20220));

	dtls_set_log_level(DTLS_LOG_DEBUG);

	dtls_context = dtls_new_context(server_conn);
	if (dtls_context) {
		dtls_set_handler(dtls_context, &cb);
	}
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF(ANSI_COLOR_GREEN);
  dtls_init();
  init_dtls();
  PRINTF(ANSI_COLOR_RESET);

  print_local_addresses();
  if (!dtls_context) {
    dtls_emerg("cannot create context\n");
    PROCESS_EXIT();
  }
  uart_set_input(1, serial_line_input_byte);

#ifdef ENABLE_POWERTRACE
  powertrace_start(CLOCK_SECOND * 2); 
#endif

  while(1) {
	  PROCESS_WAIT_EVENT();
	  if(ev == tcpip_event) {
		  PRINTF(ANSI_COLOR_GREEN);
		  PRINTF("============================\n");
		  dtls_handle_read(dtls_context);
		  PRINTF(ANSI_COLOR_RESET);
	  }
	  if (ev == serial_line_event_message && data != NULL) {
		  PRINTF(ANSI_COLOR_RED);
		  PRINTF("data ev_msg: %s\n", (char*) data);
		  PRINTF(ANSI_COLOR_RESET);
	  }
#if 0
    if (bytes_read > 0) {
      /* dtls_handle_message(dtls_context, &the_session, readbuf, bytes_read); */
      read_from_peer(dtls_context, &the_session, readbuf, bytes_read);
    }
    dtls_handle_message(ctx, &session, uip_appdata, bytes_read);
#endif
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
