	component soc_system is
		port (
			clk_clk                             : in    std_logic                    := 'X';             -- clk
			parallelport_conduit_parport_export : inout std_logic_vector(7 downto 0) := (others => 'X'); -- export
			pio_external_connection_export      : in    std_logic_vector(3 downto 0) := (others => 'X'); -- export
			reset_reset_n                       : in    std_logic                    := 'X'              -- reset_n
		);
	end component soc_system;

	u0 : component soc_system
		port map (
			clk_clk                             => CONNECTED_TO_clk_clk,                             --                          clk.clk
			parallelport_conduit_parport_export => CONNECTED_TO_parallelport_conduit_parport_export, -- parallelport_conduit_parport.export
			pio_external_connection_export      => CONNECTED_TO_pio_external_connection_export,      --      pio_external_connection.export
			reset_reset_n                       => CONNECTED_TO_reset_reset_n                        --                        reset.reset_n
		);

