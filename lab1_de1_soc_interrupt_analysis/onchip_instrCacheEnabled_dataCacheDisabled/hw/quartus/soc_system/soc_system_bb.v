
module soc_system (
	clk_clk,
	parallelport_conduit_parport_export,
	pio_external_connection_export,
	reset_reset_n);	

	input		clk_clk;
	inout	[7:0]	parallelport_conduit_parport_export;
	input	[3:0]	pio_external_connection_export;
	input		reset_reset_n;
endmodule
