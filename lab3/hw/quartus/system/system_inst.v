	system u0 (
		.clk_clk                                    (<connected-to-clk_clk>),                                    //                                 clk.clk
		.parallelport_0_conduit_parport_export      (<connected-to-parallelport_0_conduit_parport_export>),      //      parallelport_0_conduit_parport.export
		.parallelport_1_conduit_parport_export      (<connected-to-parallelport_1_conduit_parport_export>),      //      parallelport_1_conduit_parport.export
		.parallelport_common_conduit_parport_export (<connected-to-parallelport_common_conduit_parport_export>), // parallelport_common_conduit_parport.export
		.pll_0_sdram_clk                            (<connected-to-pll_0_sdram_clk>),                            //                         pll_0_sdram.clk
		.reset_reset_n                              (<connected-to-reset_reset_n>),                              //                               reset.reset_n
		.sdram_controller_common_wire_addr          (<connected-to-sdram_controller_common_wire_addr>),          //        sdram_controller_common_wire.addr
		.sdram_controller_common_wire_ba            (<connected-to-sdram_controller_common_wire_ba>),            //                                    .ba
		.sdram_controller_common_wire_cas_n         (<connected-to-sdram_controller_common_wire_cas_n>),         //                                    .cas_n
		.sdram_controller_common_wire_cke           (<connected-to-sdram_controller_common_wire_cke>),           //                                    .cke
		.sdram_controller_common_wire_cs_n          (<connected-to-sdram_controller_common_wire_cs_n>),          //                                    .cs_n
		.sdram_controller_common_wire_dq            (<connected-to-sdram_controller_common_wire_dq>),            //                                    .dq
		.sdram_controller_common_wire_dqm           (<connected-to-sdram_controller_common_wire_dqm>),           //                                    .dqm
		.sdram_controller_common_wire_ras_n         (<connected-to-sdram_controller_common_wire_ras_n>),         //                                    .ras_n
		.sdram_controller_common_wire_we_n          (<connected-to-sdram_controller_common_wire_we_n>)           //                                    .we_n
	);

