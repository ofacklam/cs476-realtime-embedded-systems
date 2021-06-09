library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity dma is
    port (
        clk:            in std_logic;
        nReset:         in std_logic;

        -- Avalon master
        address:        out std_logic_vector(31 downto 0);
        burstcount:     out std_logic_vector(4 downto 0);

        read:           out std_logic := '0';
        readdata:       in std_logic_vector(31 downto 0);
        readdatavalid:  in std_logic;

        waitrequest:    in std_logic;

        -- to rest of component
        srcStart:       in std_logic_vector(31 downto 0);
        start:          in std_logic;
        done:           out std_logic := '0';

        img_data:       out dataflow(IMAGE_SIZE-1 downto 0)
    );
end dma;

architecture comp of dma is
    constant BURST_SIZE: positive := 16;
    constant BURST_TOTAL: positive := 49;

    type SM is (Idle, StartBurst, ReadWait, ReadAccess);

    -- signals
    signal src:             std_logic_vector(31 downto 0);

    signal burst_number:    unsigned(5 downto 0);
    signal counter:         unsigned(4 downto 0);

    signal state:   SM := Idle;

begin

    pStateMachine: process(clk, nReset)

        procedure handleIdle is
        begin
            if start = '1' then
                src <= srcStart;
                burst_number <= (others => '0');
                counter <= (others => '0');
                done <= '0';
                state <= StartBurst;
            end if;
        end procedure;

        procedure handleStartBurst is
        begin
            address <= src;
            burstcount <= std_logic_vector(BURST_SIZE);
            read <= '1';
            state <= ReadWait;
        end procedure;

        procedure handleReadWait is
        begin
            if waitrequest = '0' then
                read <= '0';
                state <= ReadAccess;
            end if;
        end procedure;

        procedure processReadAccess is
        begin
            if readdatavalid = '1' then
                img_data(to_integer(burst_number) * BURST_SIZE + to_integer(counter)) <= signed(readdata);
                counter <= counter + 1;

                if counter+1 = BURST_SIZE then
                    src <= std_logic_vector(unsigned(src) + 4 * BURST_SIZE);
                    burst_number <= burst_number + 1
                    counter <= (others => '0');

                    if burst_number+1 = BURST_TOTAL then
                        state <= Idle;
                        done <= '1';
                    else
                        state <= StartBurst;
                    end if;
                end if;
            end if;
        end procedure;

    begin
        if nReset = '0' then
            read <= '0';
            done <= '0';
            state <= Idle;
        elsif rising_edge(clk) then
            case state is
                when Idle => handleIdle;
                when StartBurst => handleStartBurst;
                when ReadWait => handleReadWait;
                when ReadAccess => processReadAccess;
            end case;
        end if;
    end process pStateMachine;

end comp;
