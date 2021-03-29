	component soc_system is
		port (
			clk_clk                                 : in    std_logic                    := 'X';             -- clk
			parallelport_led_conduit_parport_export : inout std_logic_vector(9 downto 0) := (others => 'X'); -- export
			pio_btn_external_connection_export      : in    std_logic_vector(2 downto 0) := (others => 'X'); -- export
			reset_reset_n                           : in    std_logic                    := 'X'              -- reset_n
		);
	end component soc_system;

	u0 : component soc_system
		port map (
			clk_clk                                 => CONNECTED_TO_clk_clk,                                 --                              clk.clk
			parallelport_led_conduit_parport_export => CONNECTED_TO_parallelport_led_conduit_parport_export, -- parallelport_led_conduit_parport.export
			pio_btn_external_connection_export      => CONNECTED_TO_pio_btn_external_connection_export,      --      pio_btn_external_connection.export
			reset_reset_n                           => CONNECTED_TO_reset_reset_n                            --                            reset.reset_n
		);

