/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "io.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

int main()
{
  printf("Hello from Nios II!\n");

  IOWR_32DIRECT(PARALLELPORT_LED_BASE, 4*3, 0xffffffff);
  while(1) {
	  uint32_t btns = IORD_ALTERA_AVALON_PIO_DATA(PIO_BTN_BASE);
	  bool btn_val[] = {
		  btns & 1,
		  btns & 2,
		  btns & 4
	  };

	  //printf("%d %d %d\n", btn_val[0], btn_val[1], btn_val[2]);

	  uint32_t led_val = 0;
	  for(int i = 2; i >= 0; i--) {
		  led_val = led_val << 3;
		  if(btn_val[i])
			  led_val |= 7;
	  }
	  IOWR_32DIRECT(PARALLELPORT_LED_BASE, 0, led_val);
  }

  return 0;
}
