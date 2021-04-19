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

#define TIMER_MAXVAL 0xffff
#define SP_COUNTER_REGCOUNT 0
#define SP_COUNTER_REGRST 1
#define SP_COUNTER_REGSTART 2
#define SP_COUNTER_REGSTOP 3
#define SP_COUNTER_REGIEN 4
#define SP_COUNTER_CLRINT 5
#define PARPORT_REGVAL 0
#define PARPORT_REGSET 1
#define PARPORT_REGCLR 2
#define PARPORT_REGDIR 3
#define PARPORT_REGIEN 5
#define PARPORT_CLRINT 6

int done = 0;
uint16_t timestamp;

static void timer_isr_response(void *context) {
	// trigger a snapshot by writing arbitrary value
	IOWR_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE, 0);

	// get timestamp from snapshot
	timestamp = IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE);

	// clear interrupt & stop timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 1 << ALTERA_AVALON_TIMER_CONTROL_STOP_OFST);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);

	done = 1;
}

static void timer_isr_recovery(void *context) {
	// clear interrupt
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);

	// start specific counter
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGSTART*4, 1);
}

static void pio_isr_response(void *context) {
	// clear bit 0
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGCLR*4, 1);

	// clear interrupt
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_CLRINT*4, 0xffffffff);
}

static void pio_isr_recovery(void *context) {
	// disable interrupt
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGIEN*4, 0);

	// clear bit 0
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGCLR*4, 1);
}

static void pio_isr_latency(void *context) {
	// clear interrupt
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_CLRINT*4, 0xffffffff);
}

void setup_timer(int cont) {
	// clear status (pending interrupt)
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
	// stop timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 1 << ALTERA_AVALON_TIMER_CONTROL_STOP_OFST);

	// set period
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, TIMER_MAXVAL);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x0000);

	// enable interrupt & continuous mode (depending on flag)
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 1 | cont << ALTERA_AVALON_TIMER_CONTROL_CONT_OFST);
	// start timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 5 | cont << ALTERA_AVALON_TIMER_CONTROL_CONT_OFST);
}

void setup_specific_counter() {
	// clear & disable interrupt
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_CLRINT*4, 1);
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGIEN*4, 0);

	// stop & reset counter
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGSTOP*4, 1);
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGRST*4, 1);
}

void setup_parallel_port(uint32_t init_val, uint32_t intenable) {
	// disable interrupts
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGIEN*4, 0);

	// set all to output mode and initialize
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGDIR*4, 0xffffffff);
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGVAL*4, init_val);

	// clear & enable interrupts
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_CLRINT*4, 0xffffffff);
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGIEN*4, intenable);
}

void measurement_response_timer() {
	// register timer ISR
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, timer_isr_response, NULL, NULL);
	// setup timer config in continuous mode
	setup_timer(1);

	while(!done); // wait for the measurement to finish

	// print response time
	uint16_t elapsed = TIMER_MAXVAL - timestamp + 1;
	printf("Interrupt response time: %d cycles\n", elapsed);
}

void measurement_recovery_timer() {
	// register timer ISR
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, timer_isr_recovery, NULL, NULL);
	// setup timer (non-continuous mode) & counter
	setup_specific_counter();
	setup_timer(0);

	// wait for measurement to finish
	uint32_t counter_val;
	while(0 == (counter_val = IORD_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGCOUNT*4)));

	// print recovery time
	printf("Interrupt recovery time: %ld cycles\n", counter_val);
}

void measurement_pio_period() {
	// setup parallel port
	setup_parallel_port(0, 0);

	while(1) {
		IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGSET*4, 1);
		IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGCLR*4, 1);
		IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGSET*4, 1);
		IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGCLR*4, 1);
	}
}

void measurement_pio(alt_isr_func isr) {
	// register pio ISR
	alt_ic_isr_register(PARALLELPORT_0_IRQ_INTERRUPT_CONTROLLER_ID, PARALLELPORT_0_IRQ, isr, NULL, NULL);
	// setup parallel port
	setup_parallel_port(0, 0xffffffff);

	// infinite loop of setting bit 0
	while(1)
		IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGSET*4, 1);
}

int main()
{
  printf("Hello from Nios II!\n");

  //measurement_response_timer();
  //measurement_recovery_timer();
  measurement_pio(pio_isr_response);
  //measurement_pio(pio_isr_recovery);
  //measurement_pio(pio_isr_latency);

  return 0;
}
