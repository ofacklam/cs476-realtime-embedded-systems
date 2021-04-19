/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include "includes.h"
#include "system.h"
#include "io.h"
#include "altera_avalon_pio_regs.h"

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

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK_PRIORITY      1

/* Definition of synchronization primitives */
typedef struct {
	uint8_t button_number;
	uint8_t edge_rising;
	uint32_t timestamp;
} msg;

#define QUEUE_SIZE 10
msg *msg_queue[QUEUE_SIZE];

OS_EVENT *semaphore;
OS_FLAG_GRP *flags;
OS_EVENT *mailbox;
OS_EVENT *queue;
msg some_msg;

/**
 * Helper functions
 */
void reset_counter() {
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGSTOP*4, 1);
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGRST*4, 1);
}

void start_counter() {
	IOWR_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGSTART*4, 1);
}

uint32_t get_counter_value() {
	return IORD_32DIRECT(SP_COUNTER_0_BASE, SP_COUNTER_REGCOUNT*4);
}

void set_output_bit(uint8_t val) {
	uint8_t regnum = val ? PARPORT_REGSET : PARPORT_REGCLR;
	IOWR_32DIRECT(PARALLELPORT_0_BASE, regnum*4, 0x1);
}

void enable_interrupts(void (*isr)(void *ctx)) {
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_0_BASE, 0x0f); // bottom 4 bits enabled for 4 buttons
	alt_ic_isr_register(PIO_0_IRQ_INTERRUPT_CONTROLLER_ID, PIO_0_IRQ, isr, NULL, NULL);
}

uint8_t get_edge_capture(uint8_t mask) {
	uint32_t edgecap = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE);
	return edgecap & mask;
}

void clear_edge_capture() {
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_0_BASE, 0);
}

uint8_t get_input_bit(uint8_t mask) {
	uint32_t input = IORD_ALTERA_AVALON_PIO_DATA(PIO_0_BASE);
	return input & mask;
}

uint8_t get_butnum_from_edge(uint8_t edges) {
	switch(edges) {
	case 0x1:
		return 0;
	case 0x2:
		return 1;
	case 0x4:
		return 2;
	case 0x8:
		return 3;
	}
	return 0xff;
}

void create_task(void (*task)(void *parg)) {
	OSTaskCreateExt(task,
					NULL,
					(void *)&task_stk[TASK_STACKSIZE-1],
					TASK_PRIORITY,
					TASK_PRIORITY,
					task_stk,
					TASK_STACKSIZE,
					NULL,
					0);
}

/**
 * Semaphore measurements
 */
void isr_semaphore(void *context) {
	// capture only falling edge of button 0
	if(get_edge_capture(0x1) == 0 || get_input_bit(0x1) != 0)
		return;

	// clear interrupt
	clear_edge_capture();

	// signal semaphore & start measurements
	reset_counter();
	OSSemPost(semaphore);
	start_counter();
	set_output_bit(1);
}

void task_semaphore(void* pdata) {
	uint8_t err;

	while (1) {
		OSSemPend(semaphore, 0, &err); // wait for semaphore
		uint32_t elapsed = get_counter_value();
		set_output_bit(0);

		if(err == OS_ERR_NONE)
			printf("Semaphore time = %ld cycles\n", elapsed);
		else
			printf("Error getting semaphore\n");
	}
}

void semaphore_measurement() {
	// create semaphore
	semaphore = OSSemCreate(1);

	// enable interrupts
	enable_interrupts(isr_semaphore);

	// create task
	create_task(task_semaphore);
}

/**
 * Flag measurements
 */
void isr_flag_or(void *context) {
	// get all falling edges
	uint8_t edges = get_edge_capture(0x0f);
	uint8_t input = get_input_bit(0x0f);
	uint8_t falling_edges = edges & ~input;

	// clear interrupt
	clear_edge_capture();

	// signal flags & start measurements
	if(falling_edges != 0) {
		uint8_t err;
		reset_counter();
		OSFlagPost(flags, falling_edges, OS_FLAG_SET, &err);
		start_counter();
		set_output_bit(1);
	}
}

void isr_flag_and(void *context) {
	// get all falling edges
	uint8_t edges = get_edge_capture(0x0f);
	uint8_t input = get_input_bit(0x0f);
	uint8_t falling_edges = edges & ~input;

	// clear interrupt
	clear_edge_capture();

	// signal flags & start measurements
	uint8_t err;
	reset_counter();
	OS_FLAGS f = OSFlagPost(flags, falling_edges, OS_FLAG_SET, &err);
	if(f == 0x0f) {
		start_counter();
		set_output_bit(1);
	}
}

void task_flag_or(void* pdata) {
	uint8_t err;

	while (1) {
		OSFlagPend(flags, 0x0f, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
		uint32_t elapsed = get_counter_value();
		set_output_bit(0);

		if(err == OS_ERR_NONE)
			printf("Flag (OR) time = %ld cycles\n", elapsed);
		else
			printf("Error getting flags\n");
	}
}

void task_flag_and(void* pdata) {
	uint8_t err;

	while (1) {
		OSFlagPend(flags, 0x0f, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);
		uint32_t elapsed = get_counter_value();
		set_output_bit(0);

		if(err == OS_ERR_NONE)
			printf("Flag (AND) time = %ld cycles\n", elapsed);
		else
			printf("Error getting flags\n");
	}
}

void flag_measurement(uint8_t and) {
	// create flag
	uint8_t err;
	flags = OSFlagCreate(0, &err);

	// enable interrupts
	if(and)
		enable_interrupts(isr_flag_and);
	else
		enable_interrupts(isr_flag_or);

	// create task
	if(and)
		create_task(task_flag_and);
	else
		create_task(task_flag_or);
}

/**
 * Mailbox measurements
 */
void isr_mailbox(void *context) {
	// get button number & edge direction
	uint8_t edges = get_edge_capture(0x0f);
	uint8_t input = get_input_bit(edges);
	uint8_t rising = input != 0 ? 1 : 0;
	uint8_t button_number = get_butnum_from_edge(edges);

	// clear interrupt
	clear_edge_capture();

	// signal mailbox & start measurements
	reset_counter();

	some_msg.button_number = button_number;
	some_msg.edge_rising = rising;
	some_msg.timestamp = OSTimeGet();

	OSMboxPost(mailbox, &some_msg);
	start_counter();
	set_output_bit(1);
}

void task_mailbox(void* pdata) {
	uint8_t err;

	while (1) {
		msg *m = (msg*) OSMboxPend(mailbox, 0, &err);
		uint32_t elapsed = get_counter_value();
		set_output_bit(0);

		if(err == OS_ERR_NONE)
			printf("Mailbox time = %ld cycles -- button %d & edge %d -- system time = %ld ticks\n",
					elapsed, m->button_number, m->edge_rising, m->timestamp);
		else
			printf("Error getting mailbox message\n");
	}
}

void mailbox_measurement() {
	// create mailbox
	mailbox = OSMboxCreate(NULL);

	// enable interrupts
	enable_interrupts(isr_mailbox);

	// create task
	create_task(task_mailbox);
}

/**
 * Queue measurements
 */
void isr_queue(void *context) {
	// get button number & edge direction
	uint8_t edges = get_edge_capture(0x0f);
	uint8_t input = get_input_bit(edges);
	uint8_t rising = input != 0 ? 1 : 0;
	uint8_t button_number = get_butnum_from_edge(edges);

	// clear interrupt
	clear_edge_capture();

	// signal mailbox & start measurements
	reset_counter();

	some_msg.button_number = button_number;
	some_msg.edge_rising = rising;
	some_msg.timestamp = OSTimeGet();

	OSQPost(queue, &some_msg);
	start_counter();
	set_output_bit(1);
}

void task_queue(void* pdata) {
	uint8_t err;

	while (1) {
		msg *m = (msg*) OSQPend(queue, 0, &err);
		uint32_t elapsed = get_counter_value();
		set_output_bit(0);

		if(err == OS_ERR_NONE)
			printf("Queue time = %ld cycles -- button %d & edge %d -- system time = %ld ticks\n",
					elapsed, m->button_number, m->edge_rising, m->timestamp);
		else
			printf("Error getting queue message\n");
	}
}

void queue_measurement() {
	// create queue
	queue = OSQCreate((void**)msg_queue, QUEUE_SIZE);

	// enable interrupts
	enable_interrupts(isr_queue);

	// create task
	create_task(task_queue);
}


/* The main function creates two task and starts multi-tasking */
int main(void) {
	printf("Hello\n");

	//output mode on parallel port
	IOWR_32DIRECT(PARALLELPORT_0_BASE, PARPORT_REGDIR*4, 0xffffffff);

	//semaphore_measurement();
	//flag_measurement(0); //or
	//flag_measurement(1); //and
	//mailbox_measurement();
	queue_measurement();

	OSStart();
	return 0;
}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
