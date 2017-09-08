/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *       CoAP Secure (CoAPS) server example.
 * \author
 *      Raul Fuentes Samaniego <fuentess@telecom-sudparis.eu>
 *  \note 
 * 		This is an server  model which will accept any request from clients.
 * 		set_global_address() requires attention if the server IPv6
 * 		addresses will be modified.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"


//TODO RAFS: Why the CoAP Server don't use er-coap-engine?
#include "er-coaps-engine.h"

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
#endif

#include "er-coaps-transactions.h"
#include "dtls.h"
#include "tinydtls.h"

/* Used for testing different TinyDTLS versions */
#if 0
#include "dtls_debug.h" 
#else
#include "debug.h" 
#endif

#include "net/ip/uip-debug.h"




/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_mirror,
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2;
#if PLATFORM_HAS_LEDS
extern resource_t res_leds, res_toggle;
#endif
#if PLATFORM_HAS_LIGHT
#include "dev/light-sensor.h"
extern resource_t res_light;
#endif


#define LISTENING_PORT     UIP_HTONS(20220)


#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;

static dtls_context_t *dtls_context;
static uint8_t dtls_connected = 0;


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

static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: \n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
}


static int
read_from_peer(struct dtls_context_t *ctx, 
	       session_t *session, uint8 *data, size_t len) {
/* Fast testing */
#if 0  
size_t i;
  for (i = 0; i < len; i++)
    PRINTF("%c", data[i]);

  /* echo incoming application data */
  dtls_write(ctx, session, data, len);

#else 
	/* Set upip length*/
  uip_len = len;
  memmove(uip_appdata, data, len);
  coap_receive(ctx, session);
#endif
  return 0;

}

static int
send_to_peer(struct dtls_context_t *ctx, 
	     session_t *session, uint8 *data, size_t len) {

  struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

  uip_ipaddr_copy(&conn->ripaddr, &session->addr);
  conn->rport = session->port;
  
  uip_udp_packet_send(conn, data, len);

  /* Restore server connection to allow data from any node */
  memset(&conn->ripaddr, 0, sizeof(server_conn->ripaddr));
  memset(&conn->rport, 0, sizeof(conn->rport));

  return len;
}
/*---------------------------------------------------------------------------*/

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
  } psk[3] = {
    { (unsigned char *)"Client_identity", 15,
      (unsigned char *)"secretPSK", 9 },
    { (unsigned char *)"default identity", 16,
      (unsigned char *)"\x11\x22\x33", 3 },
    { (unsigned char *)"\0", 2,
      (unsigned char *)"", 1 }
  };

  if (type != DTLS_PSK_KEY) {
    return 0;
  }

  if (id) {
    int i;
    for (i = 0; i < sizeof(psk)/sizeof(struct keymap_t); i++) {
      if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
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

static void
dtls_handle_read(dtls_context_t *ctx) {
  session_t session;

  if(uip_newdata()) {
    uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
    session.port = UIP_UDP_BUF->srcport;
    session.size = sizeof(session.addr) + sizeof(session.port);
	
	    PRINTF("Server received message from ");
    PRINT6ADDR(&session.addr);
    PRINTF(":%d uip_datalen %d\n", uip_ntohs(session.port),uip_datalen());
    dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
  }
}

/*---------------------------------------------------------------------------*/
int
on_event(struct dtls_context_t *ctx, session_t *session, dtls_alert_level_t level,
              unsigned short code) {
  if (code == DTLS_EVENT_CONNECTED) {
    dtls_connected = 1;
    PRINTF("DTLS-Server Connected\n");
  }
  return 0;
}
/*---------------------------------------------------------------------------*/

void
init_dtls() {
  static dtls_handler_t cb = {
    .write = send_to_peer,
    .read  = read_from_peer,
    .event = on_event,
#ifdef DTLS_PSK
    .get_psk_info = get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
    .get_ecdsa_key = get_ecdsa_key,
    .verify_ecdsa_key = verify_ecdsa_key
#endif /* DTLS_ECC */
  };


  PRINTF("DTLS server (%s) started\n", PACKAGE_STRING);

  //FIXME: tmp_addr is a structure only defined for the cc2520
  //Lithe no usa esta linea. Queda en revision
  //memcpy(&uip_lladdr.addr, &tmp_addr.u8, 8);



  server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, LISTENING_PORT);

  dtls_set_log_level(DTLS_LOG_NOTICE);
  dtls_context = dtls_new_context(server_conn);
  if (dtls_context)
    dtls_set_handler(dtls_context, &cb);
}

/*---------------------------------------------------------------------------*/
/* Este es mas importante de lo que parece. */
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  //BLESSING  Neighbor Discovery Protocol!!!
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  /* ERROR Addresses (current issues)*/
  //uip_ip6addr(&ipaddr, 0x0, 0, 0, 0, 0, 0, 0x1200, 0x8fe);
  
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  //IPv6 Anycast and that is all, NDP do his magic
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
  
  
  //TODO: The servers  can auto-conf but adversiment the address should be good.
}
/*---------------------------------------------------------------------------*/

PROCESS_NAME(coap_server);
PROCESS(coap_server, "CoAP Server Example");
PROCESS_THREAD(coap_server, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("CoAP Server started \n");
  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
 #if REST_RES_HELLO
  rest_activate_resource(&resource_helloworld);
#endif /* REST_RES_HELLO */

#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_TOGGLE
  rest_activate_resource(&resource_toggle);
#endif /* REST_RES_TOGGLE */
#endif /* PLATFORM_HAS_LEDS */


  while(1) {
    PROCESS_YIELD();
  }
  PROCESS_END();
}


PROCESS(coaps_server_example, "CoAPS Server Example"); 
AUTOSTART_PROCESSES(&coaps_server_example);


PROCESS_THREAD(coaps_server_example, ev, data)
{
  PROCESS_BEGIN();
  //PROCESS_PAUSE(); //??

  set_global_address();

  dtls_init();
  init_dtls();

  
#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);
  
  rest_init_engine(); /* Initialize the REST engine. */
  print_local_addresses(); 
  
  if (!dtls_context) {
	  dsrv_log(LOG_EMERG, "cannot create context\n");
	PROCESS_EXIT();
  }

#if defined (DTLS_ECC)
	PRINTF("ECC OK\n");
#endif
#if defined (DTLS_PSK)
	PRINTF("PSK OK\n");
#endif  
  
  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   * RAFS NOTE: Original commented resources were erased for having a minimum.
   */
  rest_activate_resource(&res_hello, "test/hello");
  rest_activate_resource(&res_push, "test/push");
#if PLATFORM_HAS_LEDS
  rest_activate_resource(&res_toggle, "actuators/toggle");
#endif
#if PLATFORM_HAS_LIGHT
  rest_activate_resource(&res_light, "sensors/light"); 
  SENSORS_ACTIVATE(light_sensor);  
#endif
  
  
  process_start(&coap_server, NULL);
  coap_register_as_transaction_handler();


 PRINTF("DTLS server waiting requests\n");	
  while(1) {
    PROCESS_WAIT_EVENT();
	//PROCESS_YIELD();
	
	/* TODO: Validate the order of the IF's clauses */ 
    if(ev == tcpip_event) {
	 /*TESTING: Probably dtls_handle_read should not be invoked here.*/	
      dtls_handle_read(dtls_context);
	 
#if PLATFORM_HAS_BUTTON
    }else if (ev == sensors_event && data == &button_sensor){
		PRINTF("*******BUTTON*******\n");

      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      res_separate.resume();
    
#endif /* PLATFORM_HAS_BUTTON */
		
	}else if (ev == PROCESS_EVENT_TIMER) {
		 PRINTF("retransmissions\n");
      /* retransmissions are handled here */
      coap_check_transactions();
    }
  }

  PROCESS_END();
}
