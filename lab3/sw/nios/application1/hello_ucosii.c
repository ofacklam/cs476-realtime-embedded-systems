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
#include "includes.h"

#include "system.h"
#include "io.h"
#include "altera_avalon_mutex.h"
#include "altera_avalon_mailbox_simple.h"
#include "sys/alt_cache.h"
#include <stdint.h>

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       1024
OS_STK    task1_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1

/**
 * MANIPULATION 1
 */
void manip1() {
	while (1) {
		printf("Hello from uC/OS-II on CPU1\n");
		OSTimeDlyHMSM(0, 0, 3, 0);
	}
}

/**
 * MANIPULATION 2
 */
void setup() {
	IOWR_32DIRECT(PARALLELPORT_1_BASE, 3*4, 0xffffffff);
	IOWR_32DIRECT(SP_COUNTER_1_BASE, 2*4, 1);
}

void incrementBy(uint32_t base, uint8_t inc) {
	uint8_t val = IORD_32DIRECT(base, 0);
	IOWR_32DIRECT(base, 0, val+inc);
}

void safe_incrementBy(uint32_t base, uint8_t inc, alt_mutex_dev *mtx) {
	altera_avalon_mutex_lock(mtx, 1);
	incrementBy(base, inc);
	altera_avalon_mutex_unlock(mtx);
}

void manip2_part1() {
	setup();

	while(1) {
		uint32_t start = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		incrementBy(PARALLELPORT_1_BASE, 1);
		incrementBy(PARALLELPORT_COMMON_BASE, 1);
		uint32_t end = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		printf("Access time for 2 parallel port increments: %ld cycles\n", end-start);
		OSTimeDlyHMSM(0, 0, 0, 50);
	}
}

void manip2_part2() {
	setup();
	alt_mutex_dev *mtx = altera_avalon_mutex_open("/dev/mutex_io");

	while(1) {
		uint32_t start = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		incrementBy(PARALLELPORT_1_BASE, -1);
		safe_incrementBy(PARALLELPORT_COMMON_BASE, -1, mtx);
		uint32_t end = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		printf("Access time for 2 parallel port increments: %ld cycles\n", end-start);
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

/**
 * MANIPULATION 3
 */
void manip3() {
	altera_avalon_mailbox_dev *mbox = altera_avalon_mailbox_open("/dev/mailbox_common", NULL, NULL);
	alt_u32 toReceive[2];

	while(1) {
		altera_avalon_mailbox_retrieve_poll(mbox, toReceive, 0);

		alt_dcache_flush_all();
		char *msg = (void*) toReceive[1];
		printf("%s\n", msg);
	}
}

/**
 * MANIPULATION 4
 */
void atomic_incrementBy(uint32_t base, uint8_t inc) {
	IOWR_32DIRECT(base, 7*4, inc);
}

void manip4() {
	setup();

	while(1) {
		uint32_t start = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		incrementBy(PARALLELPORT_1_BASE, -1);
		atomic_incrementBy(PARALLELPORT_COMMON_BASE, -1);
		uint32_t end = IORD_32DIRECT(SP_COUNTER_1_BASE, 0);
		printf("Access time for 2 parallel port increments: %ld cycles\n", end-start);
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
}

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata) {
  manip4();
}

/* The main function creates two task and starts multi-tasking */
int main(void) {

	OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
              
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
