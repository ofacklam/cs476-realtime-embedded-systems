
module system (
	clk_clk,
	parallelport_0_conduit_parport_export,
	parallelport_1_conduit_parport_export,
	parallelport_common_conduit_parport_export,
	pll_0_sdram_clk,
	reset_reset_n,
	sdram_controller_common_wire_addr,
	sdram_controller_common_wire_ba,
	sdram_controller_common_wire_cas_n,
	sdram_controller_common_wire_cke,
	sdram_controller_common_wire_cs_n,
	sdram_controller_common_wire_dq,
	sdram_controller_common_wire_dqm,
	sdram_controller_common_wire_ras_n,
	sdram_controller_common_wire_we_n);	

	input		clk_clk;
	inout	[7:0]	parallelport_0_conduit_parport_export;
	inout	[7:0]	parallelport_1_conduit_parport_export;
	inout	[7:0]	parallelport_common_conduit_parport_export;
	output		pll_0_sdram_clk;
	input		reset_reset_n;
	output	[12:0]	sdram_controller_common_wire_addr;
	output	[1:0]	sdram_controller_common_wire_ba;
	output		sdram_controller_common_wire_cas_n;
	output		sdram_controller_common_wire_cke;
	output		sdram_controller_common_wire_cs_n;
	inout	[15:0]	sdram_controller_common_wire_dq;
	output	[1:0]	sdram_controller_common_wire_dqm;
	output		sdram_controller_common_wire_ras_n;
	output		sdram_controller_common_wire_we_n;
endmodule
