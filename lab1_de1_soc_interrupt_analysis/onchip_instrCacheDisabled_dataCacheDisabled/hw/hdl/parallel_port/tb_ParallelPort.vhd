library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_ParallelPort is
end tb_ParallelPort;

architecture test of tb_ParallelPort is

    constant CLK_PERIOD: time := 20 ns;
    constant WIDTH: positive := 20;
    constant padding: std_logic_vector(31-WIDTH downto 0) := (others => '0');
    signal test_finished: boolean := false;

    -- parallel port signals
    signal clk: std_logic;
    signal nReset: std_logic;
    signal address: std_logic_vector(2 downto 0);
    signal write: std_logic;
    signal read: std_logic;
    signal writedata: std_logic_vector(31 downto 0);
    signal readdata: std_logic_vector(31 downto 0);
    signal irq: std_logic;
    signal ParPort: std_logic_vector(WIDTH-1 downto 0);

begin

    -- instantiate the parallel port
    dut: entity work.ParallelPort
    generic map(
        width => WIDTH
    )
    port map(
        clk => clk,
        nReset => nReset,
        address => address,
        write => write,
        read => read,
        writedata => writedata,
        readdata => readdata,
        irq => irq,
        ParPort => ParPort
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

    -- test the parallel port
    simulation: process
    
            procedure async_reset is 
            begin
                wait until rising_edge(clk);
                wait for CLK_PERIOD / 4;
                nReset <= '0';
                wait for CLK_PERIOD / 2;
                nReset <= '1';
            end procedure async_reset;

            procedure writeRegister(addr: in std_logic_vector(2 downto 0); data: in std_logic_vector(31 downto 0)) is
            begin
                -- wait until rising_edge(clk);
                address <= addr;
                write <= '1';
                read <= '0';
                writedata <= data;
                wait until rising_edge(clk);
                write <= '0';
            end procedure writeRegister;

            procedure checkValue(addr: in std_logic_vector(2 downto 0); 
                                actual: in std_logic_vector(31 downto 0); 
                                expected: in std_logic_vector(31 downto 0)) is
            begin
                assert actual = expected
                report "Unexpected result: " &
                        "Register = " & integer'image(to_integer(unsigned(addr))) &
                        "Read = " & integer'image(to_integer(unsigned(actual))) &
                        "Expected = " & integer'image(to_integer(unsigned(expected)))
                severity error;
            end procedure checkValue;

            procedure testRegister(addr: in std_logic_vector(2 downto 0); expected: in std_logic_vector(31 downto 0)) is
            begin
                -- wait until rising_edge(clk);
                address <= addr;
                write <= '0';
                read <= '1';
                wait until rising_edge(clk);
                wait until rising_edge(clk); -- 1wait
                
                checkValue(addr, readdata, expected);
                
                read <= '0';
            end procedure testRegister;

            

            variable tmp: std_logic_vector(31 downto 0);

    begin

        -- default values
        nReset <= '1';
        address <= (others => '0');
        writedata <= (others => '0');
        read <= '0';
        write <= '0';
        wait for CLK_PERIOD;

        -- reset
        async_reset;

        -- test RegDir
        writeRegister("000", X"ffffffff"); -- out value
        for i in 0 to WIDTH-1 loop
            tmp := (others => '0');
            tmp(i) := '1';
            writeRegister("011", tmp);
            testRegister("011", tmp); -- read back OK

            tmp := (WIDTH-1 downto 0 => 'Z', others => '0');
            tmp(i) := '1';
            testRegister("100", tmp); -- RegPin OK
            checkValue("111", padding & ParPort, tmp); -- conduit OK
        end loop;

        -- test RegSet & RegClr
        writeRegister("011", X"ffffffff"); -- all to output mode
        for i in 0 to WIDTH-1 loop
            writeRegister("000", X"00000000"); -- test RegSet
            tmp := (others => '0');
            tmp(i) := '1';
            writeRegister("001", tmp);
            testRegister("000", tmp);

            writeRegister("000", X"ffffffff"); -- test RegClr
            writeRegister("010", tmp);
            tmp := (WIDTH-1 downto 0 => '1', others => '0');
            tmp(i) := '0';
            testRegister("000", tmp);
        end loop;

        -- test RegPort
        writeRegister("011", X"ffffffff"); -- all to output mode
        for i in 0 to WIDTH-1 loop
            tmp := (others => '0');
            tmp(i) := '1';
            writeRegister("000", tmp);
            testRegister("000", tmp); -- read back OK
            testRegister("100", tmp); -- RegPin OK
            checkValue("111", padding & ParPort, tmp); -- conduit OK
        end loop;

        -- test RegInt & RegClrInt
        writeRegister("011", X"ffffffff"); -- all to output mode
        writeRegister("000", X"00000000"); -- output 0
        writeRegister("110", X"ffffffff"); -- clear all interrupts
        for i in 0 to WIDTH-1 loop
            tmp := (others => '0'); -- set a bit
            tmp(i) := '1';
            writeRegister("001", tmp);

            wait until rising_edge(clk); -- check interrupt triggered (1 cycle latency)
            testRegister("110", tmp);

            writeRegister("010", tmp); -- clear bit

            writeRegister("110", tmp); -- clear interrupt

            wait until rising_edge(clk); -- check interrupt cleared (1 cycle latency)
            testRegister("110", X"00000000"); 
        end loop;

        -- test RegIEn & IRQ
        writeRegister("011", X"ffffffff"); -- all to output mode
        writeRegister("000", X"00000000"); -- output 0
        writeRegister("000", X"ffffffff"); -- output 1 -> trigger all interrupts
        for i in 0 to WIDTH-1 loop
            tmp := (others => '0'); -- set interrupt enable bit
            tmp(i) := '1';
            writeRegister("101", tmp);

            wait for CLK_PERIOD / 4;
            assert irq = '1' report "IRQ should be on" severity error;

            writeRegister("101", X"00000000"); -- clear interrupt enables

            wait for CLK_PERIOD / 4;
            assert irq = '0' report "IRQ should be off" severity error;
        end loop;

        -- test done
        test_finished <= true;

    end process;

end architecture test;