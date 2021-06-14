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
#include <unistd.h>

#include "system.h"
#include "io.h"
#include "i2c/i2c.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_performance_counter.h"
#include "weights.h"
#include "nn_header.h"

#define FRAME_FRAME 640
#define FRAME_LINE 20
#define LINE_FRAME 20
#define LINE_LINE 20

#define I2C_FREQ              (50000000) /* Clock frequency driving the i2c core: 50 MHz */
#define TRDB_D5M_I2C_ADDRESS  (0xba)

#define CAMCTRL_REG1_BUFNUM_OFFS 29
#define CAMCTRL_REG1_BUFSIZ_MASK 0x1fffffff

#define WIDTH 		320
#define HEIGHT 		240

#define BUF_START 	0x00
#define BUF_SIZE 	WIDTH * HEIGHT * 2
#define NUM_BUF 	1

#define INPUT_WIDTH 	16
#define INPUT_SIZE 		INPUT_WIDTH * INPUT_WIDTH * 4
#define INPUT_START 	BUF_START + BUF_SIZE
#define CUTOFF_VAL 		28

#define WEIGHTS_START	INPUT_START + INPUT_SIZE

#define LAYER2_REGNUM 	8
#define LAYER0_REGNUM 	1
#define IMGADDR_REGNUM 	12
#define CONTROL_NN_REGNUM	0

#define HIDDEN_SIZE 16
#define OUTPUT_SIZE 5

Network* software_network;
double software_input[INPUT_WIDTH * INPUT_WIDTH];


/************************************************
 * Camera setup TRDB-D5M
 ************************************************/

/**
 * From I2C demo
 */
bool trdb_d5m_write(i2c_dev *i2c, uint8_t register_offset, uint16_t data) {
    uint8_t byte_data[2] = {(data >> 8) & 0xff, data & 0xff};

    int success = i2c_write_array(i2c, TRDB_D5M_I2C_ADDRESS, register_offset, byte_data, sizeof(byte_data));

    if (success != I2C_SUCCESS) {
        return false;
    } else {
        return true;
    }
}

bool trdb_d5m_read(i2c_dev *i2c, uint8_t register_offset, uint16_t *data) {
    uint8_t byte_data[2] = {0, 0};

    int success = i2c_read_array(i2c, TRDB_D5M_I2C_ADDRESS, register_offset, byte_data, sizeof(byte_data));

    if (success != I2C_SUCCESS) {
        return false;
    } else {
        *data = ((uint16_t) byte_data[0] << 8) + byte_data[1];
        return true;
    }
}

void init_camera_hardware(bool disableBLC, bool testPattern) {
	i2c_dev i2c = I2C_INST(I2C_0);
	i2c_init(&i2c, I2C_FREQ);

	// set image height (REG 3 row size)
	trdb_d5m_write(&i2c, 3, 8*HEIGHT-1);
	// set image width (REG 4 col size)
	trdb_d5m_write(&i2c, 4, 8*WIDTH-1);

	// set row binning (REG 34 row mode)
	trdb_d5m_write(&i2c, 34, 0x0033); // row skip 4x, row bin 4x
	// set row binning (REG 35 col mode)
	trdb_d5m_write(&i2c, 35, 0x0033); // col skip 4x, col bin 4x

	// (optional) disable black level calibration
	if(disableBLC) {
		trdb_d5m_write(&i2c, 32, 0x0000); // disable row-BLC
		trdb_d5m_write(&i2c, 75, 0x0000); // digital offset 0
		trdb_d5m_write(&i2c, 98, 0x0003); // disable BLC
		trdb_d5m_write(&i2c, 96, 0x0000); // analog offset 0
		trdb_d5m_write(&i2c, 97, 0x0000); // analog offset 0
		trdb_d5m_write(&i2c, 99, 0x0000); // analog offset 0
		trdb_d5m_write(&i2c, 100, 0x0000); // analog offset 0
	}

	// (optional) set test pattern mode
	if(testPattern) {
		trdb_d5m_write(&i2c, 161, 0x0aaa);
		trdb_d5m_write(&i2c, 162, 0x0aaa);
		trdb_d5m_write(&i2c, 163, 0x0aaa);
		trdb_d5m_write(&i2c, 164, 0x00a1);
		trdb_d5m_write(&i2c, 160, 0x0041); // color field
	}

}


/*******************************************
 * Memory read / write
 *******************************************/

uint16_t get_pixel(uint32_t base, uint32_t row_idx, uint32_t col_idx, uint32_t width) {
	uint32_t addr = SDRAM_CONTROLLER_0_BASE + base + 2 * (row_idx*width + col_idx);
	return IORD_16DIRECT(addr, 0);
}

void set_pixel(uint32_t base, uint32_t row_idx, uint32_t col_idx, uint32_t width, uint16_t val) {
	uint32_t addr = SDRAM_CONTROLLER_0_BASE + base + 2 * (row_idx*width + col_idx);
	return IOWR_16DIRECT(addr, 0, val);
}

/**
 * From memory access demo
 */
void print_memory(uint32_t begin, uint32_t numRows, uint32_t numCols) {
	for(uint32_t i = 0; i < numRows; i++) {
		for(uint32_t j = 0; j < numCols; j++) {
			uint16_t pixelVal = get_pixel(begin, i, j, numCols);
			printf("%x ", pixelVal);
		}
		printf("\n");
	}
}

void set_memory(uint32_t begin, uint32_t numRows, uint32_t numCols) {
	for(uint32_t i = 0; i < numRows; i++) {
		for(uint32_t j = 0; j < numCols; j++) {
			set_pixel(begin, i, j, numCols, 0xf800);
		}
	}
}

/**
 * From https://rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C
 */
void print_to_file(char idx, uint32_t begin, uint32_t numRows, uint32_t numCols) {
	char* filename = "/mnt/host/imagex.ppm";
	filename[15] = ('0' + idx);
	FILE *fp = fopen(filename, "wb");

	// Header
	(void) fprintf(fp, "P6\n%d %d\n255\n", numCols, numRows);

	// Body
	for(uint32_t row = 0; row < numRows; row++) {
		for(uint32_t col = 0; col < numCols; col++) {
			uint16_t pixelVal = get_pixel(begin, row, col, numCols);

			uint8_t colors[3];
			colors[0] = (pixelVal >> 8) & 0xf8;
			colors[1] = (pixelVal >> 3) & 0xfc;
			colors[2] = (pixelVal << 3) & 0xf8;

			(void) fwrite(colors, sizeof(uint8_t), 3, fp);
		}
	}

	(void) fclose(fp);
	printf("Done!\n");
}

void print_to_file_transform(char idx, uint32_t begin, uint32_t numRows, uint32_t numCols) {
	char* filename = "/mnt/host/image_transformx.ppm";
	filename[15] = ('0' + idx);
	FILE *fp = fopen(filename, "wb");

	// Header
	(void) fprintf(fp, "P6\n%d %d\n255\n", numCols, numRows);

	// Body
	for(uint32_t row = 0; row < numRows; row++) {
		for(uint32_t col = 0; col < numCols; col++) {
			uint32_t pixelVal = IORD_32DIRECT(begin, 4*(row*numCols+col));

			uint8_t colors[3];
			colors[0] = pixelVal ? 255 : 0;
			colors[1] = pixelVal ? 255 : 0;
			colors[2] = pixelVal ? 255 : 0;

			(void) fwrite(colors, sizeof(uint8_t), 3, fp);
		}
	}

	(void) fclose(fp);
	printf("Done!\n");
}

/***************************************
 * Camera controller
 ***************************************/

void init_camera_controller() {
	// REG0 = base address of buf0
	IOWR_32DIRECT(CAMERA_CONTROLLER_0_BASE, 0, SDRAM_CONTROLLER_0_BASE + BUF_START);

	// REG1 = number of buffers (3 bits) + size of buffers (29 bits)
	uint32_t reg1 = (NUM_BUF << CAMCTRL_REG1_BUFNUM_OFFS) | (BUF_SIZE & CAMCTRL_REG1_BUFSIZ_MASK);
	IOWR_32DIRECT(CAMERA_CONTROLLER_0_BASE, 4, reg1);
}

void capture_image() {
	// REG2 = start the acquisition, REG3 = busy
	IOWR_32DIRECT(CAMERA_CONTROLLER_0_BASE, 8, 0x01);
	while(0 != IORD_32DIRECT(CAMERA_CONTROLLER_0_BASE, 12));
}

/***************************************
 * Utils function
 ***************************************/

void Delay_Ms(int time_ms) {
    usleep(time_ms * 1000);
    return;
}

/***************************************
 * Neural network controller
 ***************************************/

void network_init(uint32_t img_addr, uint32_t w0_addr) {
	IOWR_32DIRECT(NN_ACCELERATOR_0_BASE, LAYER0_REGNUM*4, w0_addr);
	IOWR_32DIRECT(NN_ACCELERATOR_0_BASE, IMGADDR_REGNUM*4, img_addr);
}

void set_weight(uint8_t regnum, uint16_t row, uint16_t col, uint16_t val) {
	uint32_t idx = (row << 16) | col;
	IOWR_32DIRECT(NN_ACCELERATOR_0_BASE, regnum*4, idx);
	IOWR_32DIRECT(NN_ACCELERATOR_0_BASE, (regnum+1)*4, val);
}

void set_layer2_weights(uint16_t *ws, uint16_t numRows, uint16_t numCols) {
	for(uint16_t r = 0; r < numRows; r++) {
		for(uint16_t c = 0; c < numCols; c++)
			set_weight(LAYER2_REGNUM, r, c, ws[r * numCols + c]);
	}
}

void set_layer0_weights(uint16_t *ws, uint16_t numRows, uint16_t numCols) {
	for(uint16_t c = 0; c < numCols; c++) {
		for(uint16_t r = 0; r < numRows; r++) {
			uint16_t val = ws[r * numCols + c];
			IOWR_16DIRECT(WEIGHTS_START, (c*numRows+r)*2, val);
		}
	}
}

uint16_t nn_inference() {
	// start
	IOWR_32DIRECT(NN_ACCELERATOR_0_BASE, CONTROL_NN_REGNUM*4, 0x1);

	// wait for finish
	uint32_t ctrl = 1;
	while(ctrl & 0x01)
		ctrl = IORD_32DIRECT(NN_ACCELERATOR_0_BASE, CONTROL_NN_REGNUM*4);

	// return inference value
	return (ctrl >> 16);
}

uint16_t nn_software() {
	uint16_t input_size = INPUT_WIDTH * INPUT_WIDTH;
	double* output = run_network(software_network, software_input);

	uint8_t max_idx = 0;
	double max_val = 0.0;
	for(uint8_t i = 0; i < OUTPUT_SIZE; i++) {
		if(output[i] >= max_val) {
			max_idx = i;
			max_val = output[i];
		}
	}

	return 1u << max_idx;
}

/*****************************************
 * Image pre-processing
 *****************************************/

void image_process(uint8_t software) {
	uint8_t pixelsPerBlock = HEIGHT / INPUT_WIDTH;
	uint8_t offsetLeft = (WIDTH-HEIGHT) / 2;

	for(uint8_t br = 0; br < INPUT_WIDTH; br++) {
		for(uint8_t bc = 0; bc < INPUT_WIDTH; bc++) {
			uint32_t blockval = 0;

			for(uint8_t ir = 0; ir < pixelsPerBlock; ir++) {
				for(uint8_t ic = 0; ic < pixelsPerBlock; ic++) {
					uint16_t rowidx = br * pixelsPerBlock + ir;
					uint16_t colidx = offsetLeft + bc * pixelsPerBlock + ic;
					uint16_t pixval = get_pixel(BUF_START, HEIGHT-rowidx, WIDTH-colidx, WIDTH);
					blockval += (pixval & 0x1f) + ((pixval >> 6) & 0x1f) + ((pixval >> 11) & 0x1f);
				}
			}

			uint8_t final_val = (blockval <= CUTOFF_VAL * 3 * pixelsPerBlock * pixelsPerBlock);
			if(software)
				software_input[br*INPUT_WIDTH+bc] = final_val;
			else
				IOWR_16DIRECT(INPUT_START, (br*INPUT_WIDTH+bc)*2, (final_val << 8));
		}
	}
}


/*****************************************
 * Main program
 *****************************************/

void setup() {
	// initialize the camera
	init_camera_hardware(false, false);
	Delay_Ms(2000);
	init_camera_controller();

	// initialize the neural net
	network_init(INPUT_START, WEIGHTS_START);
	set_layer0_weights(w0, HIDDEN_SIZE, INPUT_WIDTH * INPUT_WIDTH);
	set_layer2_weights(w2, OUTPUT_SIZE, HIDDEN_SIZE);

	// initialize software neural net
	software_network = create_network_from_json(JSON_NETWORK_ACTIVATION_BY_LAYERS);

	// initialize PIO
	IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_0_BASE, 0xffff);
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, 0x0);
}

void continuous_run(uint8_t software) {
	while(1) {
		//printf("capturing\n");
		capture_image();

		//printf("transforming\n");
		image_process(software);

		//printf("network inference\n");
		uint16_t onehot_val;
		if(software)
			onehot_val = nn_software();
		else
			onehot_val = nn_inference();

		//printf("diplaying result\n");
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, onehot_val);
	}
}

void profile(uint8_t software) {
	PERF_START_MEASURING(PERFORMANCE_COUNTER_0_BASE);

	int capture = 100;
	while(capture--) {
		//printf("capturing\n");
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 1);
		capture_image();
		PERF_END(PERFORMANCE_COUNTER_0_BASE, 1);

		//printf("transforming\n");
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 2);
		image_process(software);
		PERF_END(PERFORMANCE_COUNTER_0_BASE, 2);

		//printf("network inference\n");
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 3);
		uint16_t onehot_val;
		if(software)
			onehot_val = nn_software();
		else
			onehot_val = nn_inference();
		PERF_END(PERFORMANCE_COUNTER_0_BASE, 3);

		//printf("diplaying result\n");
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, onehot_val);
	}

	PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);
	perf_print_formatted_report(PERFORMANCE_COUNTER_0_BASE, alt_get_cpu_freq(), 3, "Image capture", "Image TF", "Inference");
}

int main() {
	setup();

	profile(0);

	return 0;
}

