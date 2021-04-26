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
        len:            in unsigned(31 downto 0);
        start:          in std_logic;
        remaining:      out unsigned(31 downto 0) := (others => '0');

        rdFifo:         out std_logic;
        rdFifoSize:     in std_logic_vector(6 downto 0);
        rdFifoData:     in std_logic_vector(31 downto 0);
        wrFifo:         out std_logic;
        wrFifoSize:     in std_logic_vector(6 downto 0);
        wrFifoData:     out std_logic_vector(31 downto 0)
    );
end dma;

architecture comp of dma is
    constant BURST_SIZE: positive := 16;
    constant FIFO_SIZE: positive := 64;

    type SM is (Idle, ReadWait, ReadAccess, WriteAccess);

    -- signals
    signal src:             std_logic_vector(31 downto 0);
    signal srcLen:          unsigned(31 downto 0) := (others => '0');
    signal srcBurstLen:     unsigned(4 downto 0) := (others => '0');
    signal dst:             std_logic_vector(31 downto 0);
    signal dstLen:          unsigned(31 downto 0) := (others => '0');
    signal dstBurstLen:     unsigned(4 downto 0) := (others => '0');

    signal counter:         unsigned(4 downto 0);

    signal state:   SM := Idle;

begin

    -- connect FIFO reading (acknowledge show-ahead as soon as data is written to Avalon bus)
    writedata <= rdFifoData;
    rdFifo <= '1' when state = WriteAccess and waitrequest = '0' else '0';

    -- connect FIFO writing
    wrFifoData <= readdata;
    wrFifo <= '1' when state = ReadAccess and readdatavalid = '1' else '0';

    remaining <= dstLen;

    pStateMachine: process(clk, nReset)

        function minimum(left: unsigned; right: integer) return unsigned is
            variable tmp_left: integer;
        begin
            tmp_left := to_integer(left);
            if tmp_left > right then
                return to_unsigned(right, 5);
            else
                return to_unsigned(tmp_left, 5);
            end if;
        end minimum;

        procedure handleIdle is
        begin
            if start = '1' then
                src <= srcStart;
                dst <= dstStart;
                srcLen <= len;
                dstLen <= len;
                srcBurstLen <= minimum(len, BURST_SIZE);
                dstBurstLen <= minimum(len, BURST_SIZE);
            elsif srcBurstLen > 0 and to_integer(unsigned(wrFifoSize)) + to_integer(srcBurstLen) < FIFO_SIZE then
                counter <= (others => '0');
                address <= src;
                burstcount <= std_logic_vector(srcBurstLen);
                read <= '1';
                state <= ReadWait;
            elsif dstBurstLen > 0 and to_integer(unsigned(rdFifoSize)) >= to_integer(dstBurstLen) then
                counter <= (others => '0');
                address <= dst;
                burstcount <= std_logic_vector(dstBurstLen);
                write <= '1';
                state <= WriteAccess;
            end if;
        end procedure;

        procedure processReadAccess is
        begin
            if readdatavalid = '1' then
                counter <= counter + 1;
                if counter+1 = srcBurstLen then
                    state <= Idle;
                    src <= std_logic_vector(unsigned(src) + 4 * srcBurstLen);
                    srcLen <= srcLen - srcBurstLen;
                    srcBurstLen <= minimum(srcLen - srcBurstLen, BURST_SIZE);
                end if;
            end if;
        end procedure;

        procedure processWriteAccess is
        begin
            if waitrequest = '0' then
                counter <= counter + 1;
                if counter+1 = dstBurstLen then
                    state <= Idle;
                    write <= '0';
                    dst <= std_logic_vector(unsigned(dst) + 4 * dstBurstLen);
                    dstLen <= dstLen - dstBurstLen;
                    dstBurstLen <= minimum(dstLen - dstBurstLen, BURST_SIZE);
                end if;
            end if;
        end procedure;

    begin
        if nReset = '0' then
            read <= '0';
            write <= '0';
            srcLen <= (others => '0');
            dstLen <= (others => '0');
            srcBurstLen <= (others => '0');
            dstBurstLen <= (others => '0');
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
