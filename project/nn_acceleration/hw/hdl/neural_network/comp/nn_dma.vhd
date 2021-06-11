-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity nnDma is
    port (
        clk:            in std_logic;
        nReset:         in std_logic;

        -- Avalon master
        address:        out std_logic_vector(31 downto 0);
        burstcount:     out std_logic_vector(5 downto 0);

        read:           out std_logic := '0';
        readdata:       in std_logic_vector(15 downto 0);
        readdatavalid:  in std_logic;

        waitrequest:    in std_logic;

        -- to rest of component
        srcStart:       in std_logic_vector(31 downto 0);
        weightStart:    in std_logic_vector(31 downto 0);
        start:          in std_logic;

        done:           out std_logic := '0';
        accumulate:     out std_logic := '0';
        img_data:       out fixed_point;
        weight_data:    out dataflow(HIDDEN_SIZE-1 downto 0)
    );
end nnDma;

architecture comp of nnDma is
    constant BURST_SIZE: positive := HIDDEN_SIZE;
    constant BURST_TOTAL: positive := IMAGE_SIZE;

    type SM is (Idle, StartVal, ValWait, ValRead, StartBurst, BurstWait, BurstRead);

    -- signals
    signal src:             std_logic_vector(31 downto 0);
    signal weight:          std_logic_vector(31 downto 0);

    signal burst_number:    unsigned(9 downto 0);
    signal counter:         unsigned(5 downto 0);

    signal state:   SM := Idle;

begin

    pStateMachine: process(clk, nReset)

        procedure handleIdle is
        begin
            if start = '1' then
                src <= srcStart;
                weight <= weightStart;
                burst_number <= (others => '0');
                counter <= (others => '0');
                done <= '0';
                state <= StartVal;
            end if;
        end procedure;

        procedure handleStartVal is
        begin
            address <= src;
            burstcount <= "000001";
            read <= '1';
            state <= ValWait;
        end procedure;

        procedure handleValWait is
        begin
            if waitrequest = '0' then
                read <= '0';
                state <= ValRead;
            end if;
        end procedure;

        procedure handleValRead is
        begin
            if readdatavalid = '1' then
                img_data <= signed(readdata);
                state <= StartBurst;
            end if;
        end procedure;

        procedure handleStartBurst is
        begin
            address <= weight;
            burstcount <= std_logic_vector(to_unsigned(BURST_SIZE, 6));
            read <= '1';
            state <= BurstWait;
        end procedure;

        procedure handleBurstWait is
        begin
            if waitrequest = '0' then
                read <= '0';
                state <= BurstRead;
            end if;
        end procedure;

        procedure processBurstRead is
        begin
            if readdatavalid = '1' then
                weight_data(to_integer(counter)) <= signed(readdata);
                counter <= counter + 1;

                if counter+1 = BURST_SIZE then
                    src <= std_logic_vector(unsigned(src) + 2);
                    weight <= std_logic_vector(unsigned(weight) + 2 * BURST_SIZE);
                    burst_number <= burst_number + 1;
                    counter <= (others => '0');
                    accumulate <= '1';

                    if burst_number+1 = BURST_TOTAL then
                        state <= Idle;
                        done <= '1';
                    else
                        state <= StartVal;
                    end if;
                end if;
            end if;
        end procedure;

    begin
        if nReset = '0' then
            read <= '0';
            done <= '0';
            accumulate <= '0';
            state <= Idle;
        elsif rising_edge(clk) then
            accumulate <= '0';
            case state is
                when Idle => handleIdle;
                when StartVal => handleStartVal;
                when ValWait => handleValWait;
                when ValRead => handleValRead;
                when StartBurst => handleStartBurst;
                when BurstWait => handleBurstWait;
                when BurstRead => processBurstRead;
            end case;
        end if;
    end process pStateMachine;

end comp;
