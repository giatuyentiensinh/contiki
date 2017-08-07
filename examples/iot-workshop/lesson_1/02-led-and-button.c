#include "contiki.h"

/* This is the standard input/output C library, used to print information to the
 * console, amongst other features
 */
#include <stdio.h>

/* This includes the user button library */
#include "dev/button-sensor.h"

/* And this includes the on-board LEDs */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
/* We first declare the name or our process, and create a friendly printable
 * name
 */
PROCESS(led_button_process, "LEDs and button example process");

/* And we tell the system to start this process as soon as the device is done
 * initializing
 */
AUTOSTART_PROCESSES(&led_button_process);
/*---------------------------------------------------------------------------*/
/* We declared the process in the above step, now we are implementing the
 * process.  A process communicates with other processes and, thats why we
 * include as arguments "ev" (event type) and "data" (information associated
 * with the event, so when we receive a message from another process, we can
 * check which type of event is, and depending of the type what data should I
 * receive
 */
PROCESS_THREAD(led_button_process, ev, data)
{
  /* Every process start with this macro, we tell the system this is the start
   * of the thread
   */
  PROCESS_BEGIN();

  /* Start the user button using the "SENSORS_ACTIVATE" macro */
  SENSORS_ACTIVATE(button_sensor);

  /* And now we wait for the button_sensor process to inform us about the user
   * pressing the button.  We create a loop to wait forever, but to save
   * processing cycles, we "pause" the application until the expected event is
   * received and only resume when this event occurs
   */

  while(1) {
    printf("Press the User Button\n");
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* When the user button is pressed, we toggle the LED on/off... */
    leds_toggle(LEDS_GREEN);

    /* To try different LED combinations try and replace "LEDS_GREEN" with:
     * - LEDS_RED
     * - LEDS_BLUE
     * - LEDS_ALL
     *
     * And we print its status: when zero the LED is off, else on.
     * The number printed when the sensor is on is the LED ID, this value is
     * used as a mask, to allow turning on and off multiple LED at the same
     * time (for example using "LEDS_GREEN + LEDS_RED" or "LEDS_ALL"
     */
    printf("The sensor is: %u\n", leds_get());
  }

  /* This is the end of the process, we tell the system we are done.  Even if
   * we won't reach this due to the "while(...)" we need to include it
   */
  PROCESS_END();
}
