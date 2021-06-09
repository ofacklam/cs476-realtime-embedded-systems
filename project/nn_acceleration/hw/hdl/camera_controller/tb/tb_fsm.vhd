library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_fsm is
end tb_fsm;

architecture test of tb_fsm is

    constant CLK_PERIOD: time := 100 ns;
    signal test_finished: boolean := false;

    -- FSM interface signals
    constant MAX_BUFS: positive := 4;

    signal nReset:          std_logic;
    signal clk:             std_logic;
    
    signal buf0Address:     std_logic_vector(31 downto 0);
    signal bufLength:       std_logic_vector(31 downto 0);
    signal bufNumber:       unsigned(2 downto 0);
    signal start:           std_logic;
    signal busy:            std_logic;

    signal enableCam:       std_logic;
    signal frameActive:     std_logic;

    signal empty:           std_logic;
    signal enableDMA:       std_logic;
    signal dmaBufAddr:      std_logic_vector(31 downto 0);
    signal dmaBufLen:       std_logic_vector(31 downto 0);

begin

    -- instantiate the buffer SM interface
    dut: entity work.Fsm
    generic map(
        maxBuffers => MAX_BUFS
    )
    port map(
        nReset => nReset,
        clk => clk,
        
        buf0Address => buf0Address,
        bufLength => bufLength,
        bufNumber => bufNumber,
        start => start,
        busy => busy,

        enableCam => enableCam,
        frameActive => frameActive,
        
        empty => empty,
        enableDMA => enableDMA,
        dmaBufAddr => dmaBufAddr,
        dmaBufLen => dmaBufLen
    );

    -- continuous clock signal
    clk_generation: process
    begin
        if not test_finished then
            clk <= '1';
            wait for CLK_PERIOD / 2;
            clk <= '0';
            wait for CLK_PERIOD / 2;
        else
            wait;
        end if;
    end process;

    -- test the cam interface
    simulation: process
    
            procedure async_reset is 
            begin
                wait until rising_edge(clk);
                wait for CLK_PERIOD / 4;
                nReset <= '0';
                wait for CLK_PERIOD / 2;
                nReset <= '1';
            end procedure async_reset;

            procedure checkBitValue(desc: in string;
                                    actual: in std_logic; 
                                    expected: in std_logic) is
            begin
                assert actual = expected
                report "Unexpected result for " & desc & ": " &
                        "Read = " & std_logic'image(actual) &
                        "Expected = " & std_logic'image(expected)
                severity error;
            end procedure;

            procedure check32BitValue(desc: in string;
                                    actual: in std_logic_vector(31 downto 0); 
                                    expected: in std_logic_vector(31 downto 0)) is
            begin
                assert actual = expected
                report "Unexpected result for " & desc & ": " &
                        "Read = " & integer'image(to_integer(unsigned(actual))) &
                        "Expected = " & integer'image(to_integer(unsigned(expected)))
                severity error;
            end procedure;

            procedure checkIdle is
            begin
                checkBitValue("enableCam", enableCam, '0');
                checkBitValue("enableDma", enableDma, '0');
            end procedure;

            procedure testCycle(ready: in boolean;
                                address: in std_logic_vector(31 downto 0)) is
            begin
                -- idle
                checkIdle;
                
                -- armed
                frameActive <= '1';
                wait until rising_edge(clk);
                wait until rising_edge(clk);
                wait for CLK_PERIOD/4;
                if ready then
                    checkBitValue("enableCam", enableCam, '0');
                    checkBitValue("enableDma", enableDma, '0');
                else
                    checkIdle;
                end if;

                -- enabled
                frameActive <= '0';
                wait until rising_edge(clk);
                wait for CLK_PERIOD/4;
                if ready then
                    checkBitValue("enableCam", enableCam, '1');
                    checkBitValue("enableDma", enableDma, '1');
                    check32BitValue("dmaBufAddr", dmaBufAddr, address);
                    check32BitValue("dmaBufLen", dmaBufLen, X"00000020");
                else
                    checkIdle;
                end if;

                -- busy
                frameActive <= '1';
                empty <= '0';
                wait until rising_edge(clk);
                wait for CLK_PERIOD/4;
                if ready then
                    checkBitValue("enableCam", enableCam, '1');
                    checkBitValue("enableDma", enableDma, '1');
                    check32BitValue("dmaBufAddr", dmaBufAddr, address);
                    check32BitValue("dmaBufLen", dmaBufLen, X"00000020");
                else
                    checkIdle;
                end if;

                -- flushing
                frameActive <= '0';
                wait until rising_edge(clk);
                wait for CLK_PERIOD/4;
                if ready then
                    checkBitValue("enableCam", enableCam, '0');
                    checkBitValue("enableDma", enableDma, '1');
                    check32BitValue("dmaBufAddr", dmaBufAddr, address);
                    check32BitValue("dmaBufLen", dmaBufLen, X"00000020");
                else
                    checkIdle;
                end if;

                -- end
                empty <= '1';
                wait until rising_edge(clk);
                wait for CLK_PERIOD/4;
                checkIdle;
            end procedure;

    begin

        -- default values
        nReset <= '1';
        buf0Address <= X"00000010";
        bufLength <= X"00000020";
        bufNumber <= "010"; -- 2 buffers used
        start <= '0';
        frameActive <= '0';
        empty <= '1';
        wait for CLK_PERIOD;

        -- reset
        async_reset;

        -- test buffer 0
        start <= '1';
        testCycle(true, X"00000010");
        start <= '0';

        -- test buffer 1
        start <= '1';
        testCycle(true, X"00000030");
        start <= '0';

        -- test buffer 0 (not ready)
        testCycle(false, X"00000010");

        -- test buffer 0 (ready again)
        start <= '1';
        testCycle(true, X"00000010");
        start <= '0';

        -- test done
        test_finished <= true;

    end process;

end architecture test;