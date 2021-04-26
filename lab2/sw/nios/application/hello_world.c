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
#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"

#define TEST_VALUE 0x659A659A
#define EXPECTED_VALUE 0x9AA65965
#define TEST_SIZE 1000

uint8_t done;

uint32_t values[1000];
uint32_t res[1000];

void accelerator_isr(void *ctx) {
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 2*4, 0);
	done = 1;
}

void setup(int len) {
	for(int i = 0; i < len; i++)
		IOWR_32DIRECT(&values[i], 0, TEST_VALUE);
}

bool check(int len) {
	for(int i = 0; i < len; i++)
		if(IORD_32DIRECT(&values[i], 0) != EXPECTED_VALUE)
			return false;
	return true;
}

uint32_t software_bit_manip_1(uint32_t input) {

}

void software_bit_manip(uint32_t *tab, int size) {

}

uint32_t custom_instr_bit_manip_1(uint32_t input) {
	return ALT_CI_REVERSE_INSTRUCTION_0(input, 0);
}

void custom_instr_bit_manip(uint32_t *tab, int size) {

}

void accel_bit_manip(uint32_t *tab, int size) {
	done = 0;
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 0*4, tab);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 1*4, tab);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 2*4, size);

	while(!done);
}

int main() {
	printf("Hello from Nios II!\n");

	alt_ic_isr_register(
		REVERSE_ACCELERATOR_0_IRQ_INTERRUPT_CONTROLLER_ID,
		REVERSE_ACCELERATOR_0_IRQ,
		accelerator_isr, NULL, NULL
	);

	/*done = 0;

	uint32_t val;
	IOWR_32DIRECT(&val, 0, TEST_VALUE);
	uint32_t result;
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 0*4, &val);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 1*4, &result);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 2*4, 1);
	while(!done);*/

	setup(1000);
	accel_bit_manip(values, 65);
	bool res = check(65);
	printf("Check result %d\n", res);

	return 0;
}
