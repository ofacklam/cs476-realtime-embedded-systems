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
#include "sys/alt_cache.h"
#include "altera_avalon_performance_counter.h"

#define TEST_VALUE 0x659A659A
#define EXPECTED_VALUE 0x9AA65965
#define TEST_SIZE 1000

uint32_t values[1000];

// Setup the input vector, bypassing the cache
void setup(int len) {
	alt_dcache_flush_all();
	for(int i = 0; i < len; i++)
		IOWR_32DIRECT(&values[i], 0, TEST_VALUE);
}

// Check the result vector, either in the cache or in memory
bool check(int len, bool cache) {
	for(int i = 0; i < len; i++) {
		uint32_t val = cache ? values[i] : IORD_32DIRECT(&values[i], 0);
		if(val != EXPECTED_VALUE)
			return false;
	}
	return true;
}


/**
 * Method 1 : software
 */
static uint32_t lookup_bitflip[256] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

uint32_t software_bit_manip_1(uint32_t input) {
	uint32_t h = input & 0xFF000000;
	uint32_t l = input & 0x000000FF;
	uint32_t m = (input & 0x00FFFF00) >> 8;
	uint32_t flipped_m = lookup_bitflip[m & 0x00FF] << 8 | lookup_bitflip[m >> 8];
	return l << 24 | flipped_m << 8 | h >> 24;
}

void software_bit_manip(uint32_t *tab, int size) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 1);
	for(int i = 0; i < size; i++)
		tab[i] = software_bit_manip_1(tab[i]);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, 1);
}


/**
 * Method 2: custom instruction for the NIOS-II processor
 */
uint32_t custom_instr_bit_manip_1(uint32_t input) {
	return ALT_CI_REVERSE_INSTRUCTION_0(input, 0);
}

void custom_instr_bit_manip(uint32_t *tab, int size) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 2);
	for(int i = 0; i < size; i++)
		tab[i] = custom_instr_bit_manip_1(tab[i]);
	PERF_END(PERFORMANCE_COUNTER_0_BASE, 2);
}


/**
 * Method 3: hardware accelerator
 */
void accel_bit_manip(uint32_t *tab, int size) {
	PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 3);

	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 0*4, tab);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 1*4, tab);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 2*4, size);

	while(IORD_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 3*4) == 0);
	IOWR_32DIRECT(REVERSE_ACCELERATOR_0_BASE, 2*4, 0);

	PERF_END(PERFORMANCE_COUNTER_0_BASE, 3);
}

int main() {
	printf("Hello from Nios II!\n");
	PERF_START_MEASURING(PERFORMANCE_COUNTER_0_BASE);

	bool res = false;

	/* Phase 1: only 1 value */
	/*setup(1);
	software_bit_manip(values, 1);
	res = check(1, true);
	printf("Software flip (1 value) check result %d\n", res);

	setup(1);
	custom_instr_bit_manip(values, 1);
	res = check(1, true);
	printf("Custom instr. flip (1 value) check result %d\n", res);

	setup(1);
	accel_bit_manip(values, 1);
	res = check(1, false);
	printf("Accelerator flip (1 value) check result %d\n", res);*/

	/* Phase 2: 1000 values */
	setup(1000);
	software_bit_manip(values, 1000);
	res = check(1000, true);
	printf("Software flip (1000 values) check result %d\n", res);

	setup(1000);
	custom_instr_bit_manip(values, 1000);
	res = check(1000, true);
	printf("Custom instr. flip (1000 value) check result %d\n", res);

	setup(1000);
	accel_bit_manip(values, 1000);
	res = check(1000, false);
	printf("Accelerator flip (1000 value) check result %d\n", res);

	PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);
	perf_print_formatted_report(PERFORMANCE_COUNTER_0_BASE, alt_get_cpu_freq(), 3, "Software", "Instruction", "Hardware");

	return 0;
}
