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

        rdFifo:         out std_logic;
        rdFifoSize:     in std_logic_vector(5 downto 0);
        rdFifoData:     in std_logic_vector(31 downto 0);
        wrFifo:         out std_logic;
        wrFifoSize:     in std_logic_vector(5 downto 0);
        wrFifoData:     out std_logic_vector(31 downto 0)
    );
end dma;

architecture comp of dma is
    constant BURST_SIZE: positive := 16;
    constant FIFO_SIZE: positive := 64;

    type SM is (Idle, ReadWait, ReadAccess, WriteAccess);

    -- signals
    signal src:             std_logic_vector(31 downto 0);
    signal srcLen:          unsigned(31 downto 0);
    signal srcBurstLen:     unsigned(4 downto 0);
    signal dst:             std_logic_vector(31 downto 0);
    signal dstLen:          unsigned(31 downto 0);
    signal dstBurstLen:     unsigned(4 downto 0);

    signal counter:         unsigned(4 downto 0);

    signal state:   SM;

begin

    -- connect FIFO reading (acknowledge show-ahead as soon as data is written to Avalon bus)
    writedata <= rdFifoData;
    rdFifo <= '1' when state = WriteAccess and waitrequest = '0' else '0';

    -- connect FIFO writing
    wrFifoData <= readdata;
    wrFifo <= '1' when state = ReadAccess and readdatavalid = '1' else '0';

    remaining <= dstLen;

    procedure handleIdle is
    begin
        if start = '1' then
            src <= srcStart;
            dst <= dstStart;
            srcLen <= len;
            dstLen <= len;
            srcBurstLen <= min(len, BURST_SIZE);
            dstBurstLen <= min(len, BURST_SIZE);
        elsif to_integer(unsigned(rdFifoSize)) + to_integer(burstLen) < FIFO_SIZE then
            counter <= (others => '0');
            address <= src;
            burstcount <= srcBurstLen;
            read <= '1';
            state <= ReadWait;
        elsif to_integer(wrFifoSize) >= to_integer(burstLen) then
            counter <= (others => '0');
            address <= dst;
            burstcount <= dstBurstLen;
            write <= '1';
            state <= WriteAccess;
        end if;
    end procedure;

    procedure processReadAccess is
    begin
        if readdatavalid = '1' then
            counter <= counter + 1;
            if counter+1 = burstLen then
                state <= Idle;
                src <= src + 4 * burstLen;
                srcLen <= srcLen - burstLen;
                srcBurstLen <= min(srcLen, BURST_SIZE);
            end if;
        end if;
    end procedure;

    procedure processWriteAccess is
    begin
        if waitrequest = '0' then
            counter <= counter + 1;
            if counter+1 = burstLen then
                state <= Idle;
                write <= '0';
                dst <= dst + 4 * burstLen;
                dstLen <= dstLen - burstLen;
                dstBurstLen <= min(dstLen, BURST_SIZE);
            end if;
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
                when Idle => handleIdle;
                when ReadWait => 
                    if waitrequest = '0' then
                        read <= '0';
                        state <= ReadAccess;
                    end if;
                when ReadAccess => processReadAccess;
                when WriteAccess => processWriteAccess;
            end case;
        end if;
    end process pStateMachine;

end comp;
