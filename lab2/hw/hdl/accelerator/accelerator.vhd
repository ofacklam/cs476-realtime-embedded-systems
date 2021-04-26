library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity accelerator is
    port (
        clk:            in std_logic;
        nReset:         in std_logic;

        -- Avalon master
        AM_address:         out std_logic_vector(31 downto 0);
        AM_burstcount:      out std_logic_vector(4 downto 0);

        AM_read:            out std_logic := '0';
        AM_readdata:        in std_logic_vector(31 downto 0);
        AM_readdatavalid:   in std_logic;

        AM_write:           out std_logic := '0';
        AM_writedata:       out std_logic_vector(31 downto 0);

        AM_waitrequest:     in std_logic;

        -- Avalon slave
        AS_address:     in std_logic_vector(1 downto 0);
        AS_read:        in std_logic;
        AS_readdata:    out std_logic_vector(31 downto 0);
        AS_write:       in std_logic;
        AS_writedata:   in std_logic_vector(31 downto 0);

        -- interrupt sender
        irq:            out std_logic
    );
end accelerator;

architecture comp of accelerator is

    type AccState is (Idle, Starting, Busy, IRequest);

    -- components
    component registers is
        port (
            clk:        in std_logic;
            nReset:     in std_logic;

            -- Avalon slave
            address:    in std_logic_vector(1 downto 0);
            read:       in std_logic;
            readdata:   out std_logic_vector(31 downto 0);
            write:      in std_logic;
            writedata:  in std_logic_vector(31 downto 0);

            -- to rest of component
            srcAddr:    buffer std_logic_vector(31 downto 0) := (others => '0');
            dstAddr:    buffer std_logic_vector(31 downto 0) := (others => '0');
            len:        buffer unsigned(31 downto 0) := (others => '0')
        );
    end component registers;

    component op_handler is
        port (
            -- to rest of component
            rdEmpty:        in std_logic;
            read:           out std_logic;
            inData:         in std_logic_vector(31 downto 0);

            wrFull:         in std_logic;
            write:          out std_logic;
            outData:        out std_logic_vector(31 downto 0)
        );
    end component op_handler;

    component dma is
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
            rdFifoSize:     in std_logic_vector(5 downto 0);
            rdFifoData:     in std_logic_vector(31 downto 0);
            wrFifo:         out std_logic;
            wrFifoSize:     in std_logic_vector(5 downto 0);
            wrFifoData:     out std_logic_vector(31 downto 0)
        );
    end component dma;

    component fifo is
        port
        (
            aclr		: IN STD_LOGIC ;
            clock		: IN STD_LOGIC ;
            data		: IN STD_LOGIC_VECTOR (31 DOWNTO 0);
            rdreq		: IN STD_LOGIC ;
            wrreq		: IN STD_LOGIC ;
            empty		: OUT STD_LOGIC ;
            full		: OUT STD_LOGIC ;
            q		: OUT STD_LOGIC_VECTOR (31 DOWNTO 0);
            usedw		: OUT STD_LOGIC_VECTOR (5 DOWNTO 0)
        );
    end component fifo;

    -- signals
    signal srcAddr:         std_logic_vector(31 downto 0);
    signal dstAddr:         std_logic_vector(31 downto 0);
    signal len:             unsigned(31 downto 0);

    signal inData:          std_logic_vector(31 downto 0);
    signal inRead:          std_logic;
    signal inQ:             std_logic_vector(31 downto 0);
    signal inWrite:         std_logic;
    signal inFull:          std_logic;
    signal inEmpty:         std_logic;
    signal inSize:          std_logic_vector(5 downto 0);

    signal outData:          std_logic_vector(31 downto 0);
    signal outRead:          std_logic;
    signal outQ:             std_logic_vector(31 downto 0);
    signal outWrite:         std_logic;
    signal outFull:          std_logic;
    signal outEmpty:         std_logic;
    signal outSize:          std_logic_vector(5 downto 0);

    signal start:           std_logic;
    signal remaining:       unsigned(31 downto 0);

    signal state:   AccState;

begin

    -- Avalon slave
    as: component registers
        port map (
            clk =>      clk,
            nReset =>   nReset,

            -- Avalon slave
            address =>      AS_address,
            read =>         AS_read,
            readdata =>     AS_readdata,
            write =>        AS_write,
            writedata =>    AS_writedata,

            -- to rest of component
            srcAddr =>      srcAddr,  
            dstAddr =>      dstAddr,
            len =>          len
        );

    -- 2 FIFOs
    inFifo: component fifo
        port map (
            aclr =>     not nReset,
            clock =>    clk,
            data =>     inData,
            rdreq =>    inRead,
            wrreq =>    inWrite,
            empty =>    inEmpty,
            full =>     inFull,
            q =>        inQ,
            usedw =>    inSize
        );

    outFifo: component fifo
        port map (
            aclr =>     not nReset,
            clock =>    clk,
            data =>     outData,
            rdreq =>    outRead,
            wrreq =>    outWrite,
            empty =>    outEmpty,
            full =>     outFull,
            q =>        outQ,
            usedw =>    outSize
        );

    -- Operation
    op: component op_handler
        port map (
            -- to rest of component
            rdEmpty =>      inEmpty,
            read =>         inRead,
            inData =>       inQ,

            wrFull =>       outFull,
            write =>        outWrite,
            outData =>      outData
        );

    -- DMA
    am: component dma
        port map (
            clk =>          clk,
            nReset =>       nReset,

            -- Avalon master
            address =>      AM_address,
            burstcount =>   AM_burstcount,

            read =>         AM_read,
            readdata =>     AM_readdata,
            readdatavalid =>AM_readdatavalid,

            write =>        AM_write,
            writedata =>    AM_writedata,

            waitrequest =>  AM_waitrequest,

            -- to rest of component
            srcStart =>     srcAddr,
            dstStart =>     dstAddr,
            len =>          len,
            start =>        start,
            remaining =>    remaining,

            rdFifo =>       outRead,
            rdFifoSize =>   outSize,
            rdFifoData =>   outQ,
            wrFifo =>       inWrite,
            wrFifoSize =>   inSize,
            wrFifoData =>   inData
        );

    -- state machine
    state_machine: process(clk, nReset)
    begin
        if nReset = '0' then
            irq <= '0';
            start <= '0';
            state <= Idle;
        elsif rising_edge(clk) then
            case state is
                when Idle => 
                    if len > 0 then
                        state <= Starting;
                        start <= '1';
                    end if;
                when Starting => state <= Busy;
                when Busy =>
                    start <= '0';
                    if remaining = 0 then
                        state <= IRequest;
                        irq <= '1';
                    end if;
                when IRequest =>
                    if len = 0 then
                        state <= Idle;
                        irq <= '0';
                    end if;
            end case;
        end if;
    end process;

end comp;
