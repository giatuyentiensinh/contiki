#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/uart1.h"
#include "dev/serial-line.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(serial_uart, "cc2538 serial uart process");
AUTOSTART_PROCESSES(&serial_uart);
/*---------------------------------------------------------------------------*/
unsigned int
uart1_send_bytes(const unsigned char *s, unsigned int len)
{
  unsigned int i = 0;

  while(s && *s != 0) {
    if(i >= len) {
      break;
    }
    uart_write_byte(1, *s++);
    i++;
  }
  return i;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(serial_uart, ev, data)
{
  char *rxdata;
  static uint16_t count=0;
  char string[50];
  
  PROCESS_BEGIN();
  uart_set_input(1, serial_line_input_byte);
  etimer_set(&et, CLOCK_SECOND * 4);
  leds_toggle(LEDS_GREEN);

  while(1) {
    PROCESS_WAIT_UNTIL(etimer_expired(&et));
    sprintf(string, "Msg sent from Re-mote %u.\n", ++count);
    uart1_send_bytes((uint8_t *)string, sizeof(string) - 1);
    etimer_reset(&et);
    printf("Sent " ANSI_COLOR_BLUE "%s " ANSI_COLOR_RESET "\n", string);

    PROCESS_YIELD();
    if(ev == serial_line_event_message) {
      leds_toggle(LEDS_RED);
      rxdata = data;
      printf("Data received over UART " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ".\n", rxdata);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
