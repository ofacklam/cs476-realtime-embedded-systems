
module soc_system (
	clk_clk,
	parallelport_led_conduit_parport_export,
	pio_btn_external_connection_export,
	reset_reset_n);	

	input		clk_clk;
	inout	[9:0]	parallelport_led_conduit_parport_export;
	input	[2:0]	pio_btn_external_connection_export;
	input		reset_reset_n;
endmodule
