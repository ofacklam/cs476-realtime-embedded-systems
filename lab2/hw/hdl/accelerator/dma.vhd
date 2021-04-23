library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

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

        write:          out std_logic := '0';
        writedata:      out std_logic_vector(31 downto 0);

        waitrequest:    in std_logic;

        -- to rest of component
        srcStart:       in std_logic_vector(31 downto 0);
        dstStart:       in std_logic_vector(31 downto 0);
        len:            in unsigned(31 downto 0)
        start:          in std_logic;
        remaining:      out unsigned(31 downto 0) := (others => '0');
        rdFifoSize:     in ;
        wrFifoSize:     in 
    );
end dma;

architecture comp of dma is
    constant BURST_SIZE: unsigned(4 downto 0) := 16;

    type SM is (Idle, ReadWait, ReadAccess, WriteAccess);

    -- signals
    signal src:     std_logic_vector(31 downto 0);
    signal srcLen:  unsigned(31 downto 0);
    signal dst:     std_logic_vector(31 downto 0);
    signal dstLen:  unsigned(31 downto 0);

    signal burstLen:    unsigned(4 downto 0);
    signal counter:     unsigned(4 downto 0);

    signal state:   SM;

begin

    procedure handleIdle is
    begin
        if start = '1' then
            src <= srcStart;
            dst <= dstStart;
            srcLen <= len;
            dstLen <= len;
            burstLen <= min(len, BURST_SIZE);
        elsif rdFifoSize <= burstLen then
            counter <= (others => '0');
            address <= src;
            burstcount <= burstLen;
            read <= '1';
            state <= ReadWait;
        elsif wrFifoSize >= burstLen then
            counter <= (others => '0');
            
        end if;
    end procedure;

    pStateMachine: process(clk, nReset)
    begin
        if nReset = '0' then
            read <= '0';
            write <= '0';
            remaining <= (others => '0');
            state <= Idle;
        elsif rising_edge(clk) then
            case state is
                when Idle => 
            end case;
        end if;
    end process pStateMachine;

end comp;
