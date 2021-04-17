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
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"

int done = 0;
uint16_t timestamp;

static void timer_isr(void *context) {
	// trigger a snapshot by writing arbitrary value
	IOWR_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE, 0);

	// get timestamp from snapshot
	timestamp = IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE);

	// clear interrupt & stop timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 11); //b1011
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);

	done = 1;
}

void setup_timer() {
	// clear status (pending interrupt)
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
	// stop timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 1 << ALTERA_AVALON_TIMER_CONTROL_STOP_OFST);

	// set period
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0xffff);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x0000);

	// enable continuous mode & interrupt
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 3);
	// start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 7);
}

void measurement1() {
	// register timer ISR
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, timer_isr, NULL, NULL);
	// setup timer config
	setup_timer();

	while(1) {
		if(done) {
			// print response time
			uint16_t elapsed = 0xffff - timestamp + 1;
			printf("Interrupt response time: %d cycles\n", elapsed);

			// start counter again
			//done = 0;
			//IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 7);
			break;
		}
	}
}

int main()
{
  printf("Hello from Nios II!\n");

  measurement1();

  return 0;
}
