-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity NNAccelerator is
    port (
        clk:                in std_logic;
        nReset:             in std_logic;

        -- Avalon master
        AM_address:         out std_logic_vector(31 downto 0);
        AM_burstcount:      out std_logic_vector(5 downto 0);

        AM_read:            out std_logic := '0';
        AM_readdata:        in std_logic_vector(31 downto 0);
        AM_readdatavalid:   in std_logic;

        AM_waitreq:         in std_logic;

        -- Avalon slave
        AS_address:         in std_logic_vector(3 downto 0);
        AS_read:            in std_logic;
        AS_readdata:        out std_logic_vector(31 downto 0) := (others => '0');
        AS_write:           in std_logic;
        AS_writedata:       in std_logic_vector(31 downto 0)
    );
end NNAccelerator;

architecture comp of NNAccelerator is
    type SM is (Idle, StartingDma, Reading);

    -- signals
    signal w0addr:      std_logic_vector(31 downto 0);

    --signal b0idx:       natural range 0 to 1023;
    --signal b0:          dataflow(HIDDEN_SIZE-1 downto 0);
    
    --signal w1rowidx:    natural range 0 to 1023;
    --signal w1colidx:    natural range 0 to 1023;
    --signal w1:          matrix(HIDDEN_SIZE-1 downto 0)(HIDDEN_SIZE-1 downto 0);
    
    --signal b1idx:       natural range 0 to 1023;
    --signal b1:          dataflow(HIDDEN_SIZE-1 downto 0);
    
    signal w2rowidx:    natural range 0 to 1023;
    signal w2colidx:    natural range 0 to 1023;
    signal w2:          matrix(NB_CLASSES-1 downto 0)(HIDDEN_SIZE-1 downto 0);
    
    --signal b2idx:       natural range 0 to 1023;
    --signal b2:          dataflow(NB_CLASSES-1 downto 0);

    signal start:       std_logic := '0';
    signal busy:        std_logic := '0';
    signal result:      std_logic_vector(NB_CLASSES-1 downto 0);
    signal state:       SM := Idle;
    signal img_addr:    std_logic_vector(31 downto 0);

    signal dmaStart:    std_logic := '0';
    signal dmaDone:     std_logic;
    signal accumulate:  std_logic;
    signal reset_acc:   std_logic;
    signal img_data:    fixed_point;
    signal w0:          dataflow(HIDDEN_SIZE-1 downto 0);
    signal prediction:  dataflow(NB_CLASSES-1 downto 0);

    -- components
    component Network is
        port(
            clk:            in std_logic;
            reset_acc:      in std_logic;
            accumulate:     in std_logic;

            weight0:        in dataflow(HIDDEN_SIZE-1 downto 0);
            --bias0:          in dataflow(HIDDEN_SIZE-1 downto 0);

            --weight1:        in matrix(HIDDEN_SIZE-1 downto 0)(HIDDEN_SIZE-1 downto 0);
            --bias1:          in dataflow(HIDDEN_SIZE-1 downto 0);
            
            weight2:        in matrix(NB_CLASSES-1 downto 0)(HIDDEN_SIZE-1 downto 0);
            --bias2:          in dataflow(NB_CLASSES-1 downto 0);
            
            in_data:        in fixed_point;
            out_data:       out dataflow(NB_CLASSES-1 downto 0)
        );
    end component;

    component Argmax is
        generic(
            size:           natural
        );
        port(
            in_data:        in dataflow(size-1 downto 0);
            out_data:       out std_logic_vector(size-1 downto 0)
        );
    end component;

    component nnDma is
        port (
            clk:            in std_logic;
            nReset:         in std_logic;

            -- Avalon master
            address:        out std_logic_vector(31 downto 0);
            burstcount:     out std_logic_vector(5 downto 0);

            read:           out std_logic := '0';
            readdata:       in std_logic_vector(31 downto 0);
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
    end component;

begin

    -- feed forward neural network
    reset_acc <= dmaStart or (not nReset);
    nn: Network
    port map(
        clk => clk,
        reset_acc => reset_acc,
        accumulate => accumulate,
        weight0 => w0,
        --bias0 => b0,
        --weight1 => w1,
        --bias1 => b1,
        weight2 => w2,
        --bias2 => b2,
        in_data => img_data,
        out_data => prediction
    );

    -- argmax to get prediction
    argmax_0: Argmax
    generic map(
        size => NB_CLASSES
    )
    port map(
        in_data => prediction,
        out_data => result
    );

    -- nnDma to load image from memory
    dma_0: nnDma
    port map(
        clk => clk,
        nReset => nReset,
        address => AM_address,
        burstcount => AM_burstcount,
        read => AM_read,
        readdata => AM_readdata,
        readdatavalid => AM_readdatavalid,
        waitrequest => AM_waitreq,
        srcStart => img_addr,
        weightStart => w0addr,
        start => dmaStart,
        done => dmaDone,
        accumulate => accumulate,
        img_data => img_data,
        weight_data => w0
    );

    -- Avalon slave write
    process(clk, nReset)
    begin
        if nReset = '0' then
            start <= '0';
        elsif rising_edge(clk) then
            start <= '0';
            if AS_write = '1' then
                case AS_address is
                    when "0000" => start <= '1';

                    when "0001" => w0addr <= AS_writedata;
                    --when "0010" => b0idx <= to_integer(unsigned(AS_writedata(15 downto 0)));
                    --when "0011" => b0(b0idx) <= signed(AS_writedata);

                    --when "0100" =>  w1rowidx <= to_integer(unsigned(AS_writedata(31 downto 16)));
                    --                w1colidx <= to_integer(unsigned(AS_writedata(15 downto 0)));
                    --when "0101" => w1(w1rowidx)(w1colidx) <= signed(AS_writedata);
                    --when "0110" => b1idx <= to_integer(unsigned(AS_writedata(15 downto 0)));
                    --when "0111" => b1(b1idx) <= signed(AS_writedata);

                    when "1000" =>  w2rowidx <= to_integer(unsigned(AS_writedata(31 downto 16)));
                                    w2colidx <= to_integer(unsigned(AS_writedata(15 downto 0)));
                    when "1001" => w2(w2rowidx)(w2colidx) <= signed(AS_writedata(15 downto 0));
                    --when "1010" => b2idx <= to_integer(unsigned(AS_writedata(15 downto 0)));
                    --when "1011" => b2(b2idx) <= signed(AS_writedata);

                    when "1100" => img_addr <= AS_writedata;
                    when others => null;
                end case;
            end if;
        end if;
    end process;

    -- Avalon slave read
    process(clk, nReset)
    begin
        if nReset = '0' then
            AS_readdata <= (others => '0');
        elsif rising_edge(clk) then
            AS_readdata <= (others => '0');
            if AS_read = '1' then
                case AS_address is
                    when "0000" => AS_readdata <= "00000000000" & result & "000000000000000" & busy;

                    when "0001" => AS_readdata <= w0addr;
                    --when "0010" => AS_readdata <= std_logic_vector(to_unsigned(b0idx, 32));
                    --when "0011" => AS_readdata <= std_logic_vector(b0(b0idx));
                    
                    --when "0100" => AS_readdata <= std_logic_vector(to_unsigned(w1rowidx, 16)) & std_logic_vector(to_unsigned(w1colidx, 16));
                    --when "0101" => AS_readdata <= std_logic_vector(w1(w1rowidx)(w1colidx));
                    --when "0110" => AS_readdata <= std_logic_vector(to_unsigned(b1idx, 32));
                    --when "0111" => AS_readdata <= std_logic_vector(b1(b1idx));

                    when "1000" => AS_readdata <= std_logic_vector(to_unsigned(w2rowidx, 16)) & std_logic_vector(to_unsigned(w2colidx, 16));
                    when "1001" => AS_readdata <= X"0000" & std_logic_vector(w2(w2rowidx)(w2colidx));
                    --when "1010" => AS_readdata <= std_logic_vector(to_unsigned(b2idx, 32));
                    --when "1011" => AS_readdata <= std_logic_vector(b2(b2idx));

                    when "1100" => AS_readdata <= img_addr;
                    when others => null;
                end case;
            end if;
        end if;
    end process;

    -- State machine
    pStateMachine: process(clk, nReset)

        procedure handleIdle is
        begin
            if start = '1' then
                dmaStart <= '1';
                busy <= '1';
                state <= StartingDma;
            end if;
        end procedure;

        procedure handleStartingDma is
        begin
            if dmaDone = '0' then
                state <= Reading;
            end if;
        end procedure;

        procedure handleReading is
        begin
            dmaStart <= '0';
            if dmaDone = '1' then
                busy <= '0';
                state <= Idle;
            end if;
        end procedure;

    begin
        if nReset = '0' then
            dmaStart <= '0';
            busy <= '0';
            state <= Idle;
        elsif rising_edge(clk) then
            case state is
                when Idle => handleIdle;
                when StartingDma => handleStartingDma;
                when Reading => handleReading;
            end case;
        end if;
    end process pStateMachine;

end comp;
