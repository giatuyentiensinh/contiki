/*
 * Copyright (c) 2013, Todo
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
 *      IPv6 HC06 CoAPS (Normal UDP)
 * \author
 *      Raul Armando Fuentes Samaniego <fuentess@telecom...>
 */

//Just a fast way to disable this configuration and use default one.
// #ifndef PROJECT_6COAPS_TINYDTLS_CONF_H_
// #define PROJECT_6COAPS_TINYDTLS_CONF_H_
// #endif 


#ifndef PROJECT_6COAPS_TINYDTLS_CONF_H_
#define PROJECT_6COAPS_TINYDTLS_CONF_H_

/*  Basic configuration for the nodes. 
 *  802.15.4 for the framers. 
 *  CSMA/CA for the transmission 
 *  The CC24240 drivers for the signal 
 *  And the default (contikimac)  RDC driver (not stated here) 
 */

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154


/* TODO: Testing those parameters */
// #undef NETSTACK_CONF_MAC
// #define NETSTACK_CONF_MAC     csma_driver

/* This will disable the RPL protocol. (Not wished but useful for testing)  */
// #undef UIP_CONF_IPV6_RPL
// #define UIP_CONF_IPV6_RPL 0

/* UIP_CONF_ND6_SEND_NA enables standard IPv6 Neighbor Discovery Protocol.
   This is unneeded when RPL is used. Disable to save ROM and a little RAM. 
   NOTE: Is a lie! The nodes are unable to connect if its disabled.   
*/
// #undef UIP_CONF_ND6_SEND_NA
// #define UIP_CONF_ND6_SEND_NA 1


/*  NHC (compression for UDP). NOTE: There are 4 possible configurations.  */
#undef SICSLOWPAN_CONF_COMPRESSION
#define SICSLOWPAN_CONF_COMPRESSION     SICSLOWPAN_COMPRESSION_HC06 


/* Disabling TCP on CoAP nodes. */
#undef UIP_CONF_TCP
#define UIP_CONF_TCP                   0


/* Increase rpl-border-router IP-buffer when using more than 64. */
// #undef REST_MAX_CHUNK_SIZE
// #define REST_MAX_CHUNK_SIZE            48


/* Hardware of the mote  */
/* NOTHING: This is handled by each mote's configuration. */

/* CoAP Configuration */
/* Multiplies with chunk size, be aware of memory constraints. */
#undef COAP_MAX_OPEN_TRANSACTIONS
#define COAP_MAX_OPEN_TRANSACTIONS     4

/* Filtering .well-known/core per query can be disabled to save space.
#undef COAP_LINK_FORMAT_FILTERING
#define COAP_LINK_FORMAT_FILTERING     0
#undef COAP_PROXY_OPTION_PROCESSING
#define COAP_PROXY_OPTION_PROCESSING   0  */

/* DTLS configuration  */

/** Number of message retransmissions. (Default 7) */
#undef DTLS_DEFAULT_MAX_RETRANSMIT
#define DTLS_DEFAULT_MAX_RETRANSMIT 7

#endif
