
module system (
	camera_controller_0_camera_conduit_data,
	camera_controller_0_camera_conduit_fval,
	camera_controller_0_camera_conduit_lval,
	camera_controller_0_camera_conduit_pixclk,
	clk_clk,
	i2c_0_i2c_scl,
	i2c_0_i2c_sda,
	pio_0_external_connection_export,
	pll_0_sdram_clk,
	reset_reset_n,
	sdram_controller_0_wire_addr,
	sdram_controller_0_wire_ba,
	sdram_controller_0_wire_cas_n,
	sdram_controller_0_wire_cke,
	sdram_controller_0_wire_cs_n,
	sdram_controller_0_wire_dq,
	sdram_controller_0_wire_dqm,
	sdram_controller_0_wire_ras_n,
	sdram_controller_0_wire_we_n);	

	input	[11:0]	camera_controller_0_camera_conduit_data;
	input		camera_controller_0_camera_conduit_fval;
	input		camera_controller_0_camera_conduit_lval;
	input		camera_controller_0_camera_conduit_pixclk;
	input		clk_clk;
	inout		i2c_0_i2c_scl;
	inout		i2c_0_i2c_sda;
	output	[9:0]	pio_0_external_connection_export;
	output		pll_0_sdram_clk;
	input		reset_reset_n;
	output	[12:0]	sdram_controller_0_wire_addr;
	output	[1:0]	sdram_controller_0_wire_ba;
	output		sdram_controller_0_wire_cas_n;
	output		sdram_controller_0_wire_cke;
	output		sdram_controller_0_wire_cs_n;
	inout	[15:0]	sdram_controller_0_wire_dq;
	output	[1:0]	sdram_controller_0_wire_dqm;
	output		sdram_controller_0_wire_ras_n;
	output		sdram_controller_0_wire_we_n;
endmodule
