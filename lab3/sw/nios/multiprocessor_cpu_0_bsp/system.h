/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'cpu_0' in SOPC Builder design 'system'
 * SOPC Builder design path: /home/vm/Documents/cs476/lab3/hw/quartus/system.sopcinfo
 *
 * Generated: Mon May 17 10:48:00 CEST 2021
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x04040820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 50000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x1b
#define ALT_CPU_DCACHE_BYPASS_MASK 0x80000000
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x04020020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 50000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_EXTRA_EXCEPTION_INFO
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 4096
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x1b
#define ALT_CPU_NAME "cpu_0"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x04020000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x04040820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 50000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x1b
#define NIOS2_DCACHE_BYPASS_MASK 0x80000000
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x04020020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_EXTRA_EXCEPTION_INFO
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x1b
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x04020000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_MAILBOX_SIMPLE
#define __ALTERA_AVALON_MUTEX
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PERFORMANCE_COUNTER
#define __ALTERA_AVALON_TIMER
#define __ALTERA_NIOS2_GEN2
#define __PARALLELPORT
#define __SP_COUNTER


/*
 * ParallelPort_0 configuration
 *
 */

#define ALT_MODULE_CLASS_ParallelPort_0 ParallelPort
#define PARALLELPORT_0_BASE 0x4041080
#define PARALLELPORT_0_IRQ 2
#define PARALLELPORT_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define PARALLELPORT_0_NAME "/dev/ParallelPort_0"
#define PARALLELPORT_0_SPAN 32
#define PARALLELPORT_0_TYPE "ParallelPort"


/*
 * ParallelPort_common configuration
 *
 */

#define ALT_MODULE_CLASS_ParallelPort_common ParallelPort
#define PARALLELPORT_COMMON_BASE 0x4041060
#define PARALLELPORT_COMMON_IRQ -1
#define PARALLELPORT_COMMON_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PARALLELPORT_COMMON_NAME "/dev/ParallelPort_common"
#define PARALLELPORT_COMMON_SPAN 32
#define PARALLELPORT_COMMON_TYPE "ParallelPort"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone V"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_BASE 0x40410e0
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_BASE 0x40410e0
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_BASE 0x40410e0
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "system"


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 32
#define ALT_SYS_CLK TIMER_0
#define ALT_TIMESTAMP_CLK none


/*
 * internal_memory_0 configuration
 *
 */

#define ALT_MODULE_CLASS_internal_memory_0 altera_avalon_onchip_memory2
#define INTERNAL_MEMORY_0_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define INTERNAL_MEMORY_0_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define INTERNAL_MEMORY_0_BASE 0x4020000
#define INTERNAL_MEMORY_0_CONTENTS_INFO ""
#define INTERNAL_MEMORY_0_DUAL_PORT 0
#define INTERNAL_MEMORY_0_GUI_RAM_BLOCK_TYPE "AUTO"
#define INTERNAL_MEMORY_0_INIT_CONTENTS_FILE "system_internal_memory_0"
#define INTERNAL_MEMORY_0_INIT_MEM_CONTENT 1
#define INTERNAL_MEMORY_0_INSTANCE_ID "NONE"
#define INTERNAL_MEMORY_0_IRQ -1
#define INTERNAL_MEMORY_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define INTERNAL_MEMORY_0_NAME "/dev/internal_memory_0"
#define INTERNAL_MEMORY_0_NON_DEFAULT_INIT_FILE_ENABLED 0
#define INTERNAL_MEMORY_0_RAM_BLOCK_TYPE "AUTO"
#define INTERNAL_MEMORY_0_READ_DURING_WRITE_MODE "DONT_CARE"
#define INTERNAL_MEMORY_0_SINGLE_CLOCK_OP 0
#define INTERNAL_MEMORY_0_SIZE_MULTIPLE 1
#define INTERNAL_MEMORY_0_SIZE_VALUE 131072
#define INTERNAL_MEMORY_0_SPAN 131072
#define INTERNAL_MEMORY_0_TYPE "altera_avalon_onchip_memory2"
#define INTERNAL_MEMORY_0_WRITABLE 1


/*
 * jtag_uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart
#define JTAG_UART_0_BASE 0x40410e0
#define JTAG_UART_0_IRQ 3
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8


/*
 * mailbox_common configuration
 *
 */

#define ALT_MODULE_CLASS_mailbox_common altera_avalon_mailbox_simple
#define MAILBOX_COMMON_BASE 0x40410c0
#define MAILBOX_COMMON_IRQ -1
#define MAILBOX_COMMON_IRQ_INTERRUPT_CONTROLLER_ID -1
#define MAILBOX_COMMON_NAME "/dev/mailbox_common"
#define MAILBOX_COMMON_SPAN 16
#define MAILBOX_COMMON_TYPE "altera_avalon_mailbox_simple"


/*
 * mutex_io configuration
 *
 */

#define ALT_MODULE_CLASS_mutex_io altera_avalon_mutex
#define MUTEX_IO_BASE 0x40410d8
#define MUTEX_IO_IRQ -1
#define MUTEX_IO_IRQ_INTERRUPT_CONTROLLER_ID -1
#define MUTEX_IO_NAME "/dev/mutex_io"
#define MUTEX_IO_OWNER_INIT 0
#define MUTEX_IO_OWNER_WIDTH 16
#define MUTEX_IO_SPAN 8
#define MUTEX_IO_TYPE "altera_avalon_mutex"
#define MUTEX_IO_VALUE_INIT 0
#define MUTEX_IO_VALUE_WIDTH 16


/*
 * mutex_mailbox configuration
 *
 */

#define ALT_MODULE_CLASS_mutex_mailbox altera_avalon_mutex
#define MUTEX_MAILBOX_BASE 0x40410d0
#define MUTEX_MAILBOX_IRQ -1
#define MUTEX_MAILBOX_IRQ_INTERRUPT_CONTROLLER_ID -1
#define MUTEX_MAILBOX_NAME "/dev/mutex_mailbox"
#define MUTEX_MAILBOX_OWNER_INIT 0
#define MUTEX_MAILBOX_OWNER_WIDTH 16
#define MUTEX_MAILBOX_SPAN 8
#define MUTEX_MAILBOX_TYPE "altera_avalon_mutex"
#define MUTEX_MAILBOX_VALUE_INIT 0
#define MUTEX_MAILBOX_VALUE_WIDTH 16


/*
 * performance_counter_0 configuration
 *
 */

#define ALT_MODULE_CLASS_performance_counter_0 altera_avalon_performance_counter
#define PERFORMANCE_COUNTER_0_BASE 0x4041000
#define PERFORMANCE_COUNTER_0_HOW_MANY_SECTIONS 3
#define PERFORMANCE_COUNTER_0_IRQ -1
#define PERFORMANCE_COUNTER_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PERFORMANCE_COUNTER_0_NAME "/dev/performance_counter_0"
#define PERFORMANCE_COUNTER_0_SPAN 64
#define PERFORMANCE_COUNTER_0_TYPE "altera_avalon_performance_counter"


/*
 * sdram_controller_common configuration
 *
 */

#define ALT_MODULE_CLASS_sdram_controller_common altera_avalon_new_sdram_controller
#define SDRAM_CONTROLLER_COMMON_BASE 0x0
#define SDRAM_CONTROLLER_COMMON_CAS_LATENCY 3
#define SDRAM_CONTROLLER_COMMON_CONTENTS_INFO
#define SDRAM_CONTROLLER_COMMON_INIT_NOP_DELAY 0.0
#define SDRAM_CONTROLLER_COMMON_INIT_REFRESH_COMMANDS 2
#define SDRAM_CONTROLLER_COMMON_IRQ -1
#define SDRAM_CONTROLLER_COMMON_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDRAM_CONTROLLER_COMMON_IS_INITIALIZED 1
#define SDRAM_CONTROLLER_COMMON_NAME "/dev/sdram_controller_common"
#define SDRAM_CONTROLLER_COMMON_POWERUP_DELAY 100.0
#define SDRAM_CONTROLLER_COMMON_REFRESH_PERIOD 7.8125
#define SDRAM_CONTROLLER_COMMON_REGISTER_DATA_IN 1
#define SDRAM_CONTROLLER_COMMON_SDRAM_ADDR_WIDTH 0x19
#define SDRAM_CONTROLLER_COMMON_SDRAM_BANK_WIDTH 2
#define SDRAM_CONTROLLER_COMMON_SDRAM_COL_WIDTH 10
#define SDRAM_CONTROLLER_COMMON_SDRAM_DATA_WIDTH 16
#define SDRAM_CONTROLLER_COMMON_SDRAM_NUM_BANKS 4
#define SDRAM_CONTROLLER_COMMON_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_CONTROLLER_COMMON_SDRAM_ROW_WIDTH 13
#define SDRAM_CONTROLLER_COMMON_SHARED_DATA 0
#define SDRAM_CONTROLLER_COMMON_SIM_MODEL_BASE 0
#define SDRAM_CONTROLLER_COMMON_SPAN 67108864
#define SDRAM_CONTROLLER_COMMON_STARVATION_INDICATOR 0
#define SDRAM_CONTROLLER_COMMON_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_CONTROLLER_COMMON_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_CONTROLLER_COMMON_T_AC 5.4
#define SDRAM_CONTROLLER_COMMON_T_MRD 3
#define SDRAM_CONTROLLER_COMMON_T_RCD 15.0
#define SDRAM_CONTROLLER_COMMON_T_RFC 70.0
#define SDRAM_CONTROLLER_COMMON_T_RP 15.0
#define SDRAM_CONTROLLER_COMMON_T_WR 14.0


/*
 * sp_counter_0 configuration
 *
 */

#define ALT_MODULE_CLASS_sp_counter_0 sp_counter
#define SP_COUNTER_0_BASE 0x40410a0
#define SP_COUNTER_0_IRQ 1
#define SP_COUNTER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define SP_COUNTER_0_NAME "/dev/sp_counter_0"
#define SP_COUNTER_0_SPAN 32
#define SP_COUNTER_0_TYPE "sp_counter"


/*
 * timer_0 configuration
 *
 */

#define ALT_MODULE_CLASS_timer_0 altera_avalon_timer
#define TIMER_0_ALWAYS_RUN 0
#define TIMER_0_BASE 0x4041040
#define TIMER_0_COUNTER_SIZE 32
#define TIMER_0_FIXED_PERIOD 0
#define TIMER_0_FREQ 50000000
#define TIMER_0_IRQ 0
#define TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_0_LOAD_VALUE 4999
#define TIMER_0_MULT 1.0E-6
#define TIMER_0_NAME "/dev/timer_0"
#define TIMER_0_PERIOD 100
#define TIMER_0_PERIOD_UNITS "us"
#define TIMER_0_RESET_OUTPUT 0
#define TIMER_0_SNAPSHOT 1
#define TIMER_0_SPAN 32
#define TIMER_0_TICKS_PER_SEC 10000
#define TIMER_0_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_0_TYPE "altera_avalon_timer"


/*
 * ucosii configuration
 *
 */

#define OS_ARG_CHK_EN 1
#define OS_CPU_HOOKS_EN 1
#define OS_DEBUG_EN 1
#define OS_EVENT_NAME_SIZE 32
#define OS_FLAGS_NBITS 16
#define OS_FLAG_ACCEPT_EN 1
#define OS_FLAG_DEL_EN 1
#define OS_FLAG_EN 1
#define OS_FLAG_NAME_SIZE 32
#define OS_FLAG_QUERY_EN 1
#define OS_FLAG_WAIT_CLR_EN 1
#define OS_LOWEST_PRIO 20
#define OS_MAX_EVENTS 60
#define OS_MAX_FLAGS 20
#define OS_MAX_MEM_PART 60
#define OS_MAX_QS 20
#define OS_MAX_TASKS 10
#define OS_MBOX_ACCEPT_EN 1
#define OS_MBOX_DEL_EN 1
#define OS_MBOX_EN 1
#define OS_MBOX_POST_EN 1
#define OS_MBOX_POST_OPT_EN 1
#define OS_MBOX_QUERY_EN 1
#define OS_MEM_EN 1
#define OS_MEM_NAME_SIZE 32
#define OS_MEM_QUERY_EN 1
#define OS_MUTEX_ACCEPT_EN 1
#define OS_MUTEX_DEL_EN 1
#define OS_MUTEX_EN 1
#define OS_MUTEX_QUERY_EN 1
#define OS_Q_ACCEPT_EN 1
#define OS_Q_DEL_EN 1
#define OS_Q_EN 1
#define OS_Q_FLUSH_EN 1
#define OS_Q_POST_EN 1
#define OS_Q_POST_FRONT_EN 1
#define OS_Q_POST_OPT_EN 1
#define OS_Q_QUERY_EN 1
#define OS_SCHED_LOCK_EN 1
#define OS_SEM_ACCEPT_EN 1
#define OS_SEM_DEL_EN 1
#define OS_SEM_EN 1
#define OS_SEM_QUERY_EN 1
#define OS_SEM_SET_EN 1
#define OS_TASK_CHANGE_PRIO_EN 1
#define OS_TASK_CREATE_EN 1
#define OS_TASK_CREATE_EXT_EN 1
#define OS_TASK_DEL_EN 1
#define OS_TASK_IDLE_STK_SIZE 512
#define OS_TASK_NAME_SIZE 32
#define OS_TASK_PROFILE_EN 1
#define OS_TASK_QUERY_EN 1
#define OS_TASK_STAT_EN 1
#define OS_TASK_STAT_STK_CHK_EN 1
#define OS_TASK_STAT_STK_SIZE 512
#define OS_TASK_SUSPEND_EN 1
#define OS_TASK_SW_HOOK_EN 1
#define OS_TASK_TMR_PRIO 0
#define OS_TASK_TMR_STK_SIZE 512
#define OS_THREAD_SAFE_NEWLIB 1
#define OS_TICKS_PER_SEC TIMER_0_TICKS_PER_SEC
#define OS_TICK_STEP_EN 1
#define OS_TIME_DLY_HMSM_EN 1
#define OS_TIME_DLY_RESUME_EN 1
#define OS_TIME_GET_SET_EN 1
#define OS_TIME_TICK_HOOK_EN 1
#define OS_TMR_CFG_MAX 16
#define OS_TMR_CFG_NAME_SIZE 16
#define OS_TMR_CFG_TICKS_PER_SEC 10
#define OS_TMR_CFG_WHEEL_SIZE 2
#define OS_TMR_EN 0

#endif /* __SYSTEM_H_ */
