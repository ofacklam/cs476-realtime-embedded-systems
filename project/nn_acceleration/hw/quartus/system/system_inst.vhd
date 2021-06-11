	component system is
		port (
			camera_controller_0_camera_conduit_data   : in    std_logic_vector(11 downto 0) := (others => 'X'); -- data
			camera_controller_0_camera_conduit_fval   : in    std_logic                     := 'X';             -- fval
			camera_controller_0_camera_conduit_lval   : in    std_logic                     := 'X';             -- lval
			camera_controller_0_camera_conduit_pixclk : in    std_logic                     := 'X';             -- pixclk
			clk_clk                                   : in    std_logic                     := 'X';             -- clk
			i2c_0_i2c_scl                             : inout std_logic                     := 'X';             -- scl
			i2c_0_i2c_sda                             : inout std_logic                     := 'X';             -- sda
			pio_0_external_connection_export          : out   std_logic_vector(9 downto 0);                     -- export
			pll_0_sdram_clk                           : out   std_logic;                                        -- clk
			reset_reset_n                             : in    std_logic                     := 'X';             -- reset_n
			sdram_controller_0_wire_addr              : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_controller_0_wire_ba                : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_controller_0_wire_cas_n             : out   std_logic;                                        -- cas_n
			sdram_controller_0_wire_cke               : out   std_logic;                                        -- cke
			sdram_controller_0_wire_cs_n              : out   std_logic;                                        -- cs_n
			sdram_controller_0_wire_dq                : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_controller_0_wire_dqm               : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_controller_0_wire_ras_n             : out   std_logic;                                        -- ras_n
			sdram_controller_0_wire_we_n              : out   std_logic                                         -- we_n
		);
	end component system;

	u0 : component system
		port map (
			camera_controller_0_camera_conduit_data   => CONNECTED_TO_camera_controller_0_camera_conduit_data,   -- camera_controller_0_camera_conduit.data
			camera_controller_0_camera_conduit_fval   => CONNECTED_TO_camera_controller_0_camera_conduit_fval,   --                                   .fval
			camera_controller_0_camera_conduit_lval   => CONNECTED_TO_camera_controller_0_camera_conduit_lval,   --                                   .lval
			camera_controller_0_camera_conduit_pixclk => CONNECTED_TO_camera_controller_0_camera_conduit_pixclk, --                                   .pixclk
			clk_clk                                   => CONNECTED_TO_clk_clk,                                   --                                clk.clk
			i2c_0_i2c_scl                             => CONNECTED_TO_i2c_0_i2c_scl,                             --                          i2c_0_i2c.scl
			i2c_0_i2c_sda                             => CONNECTED_TO_i2c_0_i2c_sda,                             --                                   .sda
			pio_0_external_connection_export          => CONNECTED_TO_pio_0_external_connection_export,          --          pio_0_external_connection.export
			pll_0_sdram_clk                           => CONNECTED_TO_pll_0_sdram_clk,                           --                        pll_0_sdram.clk
			reset_reset_n                             => CONNECTED_TO_reset_reset_n,                             --                              reset.reset_n
			sdram_controller_0_wire_addr              => CONNECTED_TO_sdram_controller_0_wire_addr,              --            sdram_controller_0_wire.addr
			sdram_controller_0_wire_ba                => CONNECTED_TO_sdram_controller_0_wire_ba,                --                                   .ba
			sdram_controller_0_wire_cas_n             => CONNECTED_TO_sdram_controller_0_wire_cas_n,             --                                   .cas_n
			sdram_controller_0_wire_cke               => CONNECTED_TO_sdram_controller_0_wire_cke,               --                                   .cke
			sdram_controller_0_wire_cs_n              => CONNECTED_TO_sdram_controller_0_wire_cs_n,              --                                   .cs_n
			sdram_controller_0_wire_dq                => CONNECTED_TO_sdram_controller_0_wire_dq,                --                                   .dq
			sdram_controller_0_wire_dqm               => CONNECTED_TO_sdram_controller_0_wire_dqm,               --                                   .dqm
			sdram_controller_0_wire_ras_n             => CONNECTED_TO_sdram_controller_0_wire_ras_n,             --                                   .ras_n
			sdram_controller_0_wire_we_n              => CONNECTED_TO_sdram_controller_0_wire_we_n               --                                   .we_n
		);

