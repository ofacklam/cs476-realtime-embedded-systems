library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity ParallelPort is
    generic (
        width:      positive range 1 to 32 := 32
    );
    port (
        -- Avalon signals
        clk:        in std_logic;
        nReset:     in std_logic;

        address:    in std_logic_vector(2 downto 0);

        read:       in std_logic;
        readdata:   out std_logic_vector(31 downto 0);

        write:      in std_logic;
        writedata:  in std_logic_vector(31 downto 0);

        irq:        out std_logic;

        -- Conduit
        ParPort:    inout std_logic_vector(width-1 downto 0)
    );
end ParallelPort;


architecture comp of ParallelPort is

    -- internal signals
    signal iRegDir:         std_logic_vector(width-1 downto 0) := (others => '0');
    signal iRegPort:        std_logic_vector(width-1 downto 0) := (others => '0');
    signal iRegPin:         std_logic_vector(width-1 downto 0);
    signal iRegPinOld:      std_logic_vector(width-1 downto 0) := (others => '0');
    signal iOldValid:       std_logic := '0';
    signal iRegIEn:         std_logic_vector(width-1 downto 0) := (others => '0');
    signal iRegInt:         std_logic_vector(width-1 downto 0) := (others => '0');
    signal iRegClrInt:      std_logic_vector(width-1 downto 0) := (others => '0');

begin

    -- drive the conduit signal
    pPort: process(iRegDir, iRegPort)
    begin
        for i in 0 to width-1 loop
            if iRegDir(i) = '1' then
                ParPort(i) <= iRegPort(i);
            else
                ParPort(i) <= 'Z';
            end if;
        end loop;
    end process pPort;

    -- drive the input register
    iRegPin <= ParPort;

    -- Avalon write
    pWrite: process(clk, nReset)
    begin
        if nReset = '0' then
            iRegDir <= (others => '0');
            iRegPort <= (others => '0');
            iRegIEn <= (others => '0');
            iRegClrInt <= (others => '0');
        elsif rising_edge(clk) then
            iRegClrInt <= (others => '0');
            if write = '1' then
                case address is
                    when "000" => iRegPort <= writedata(width-1 downto 0);
                    when "001" => iRegPort <= iRegPort or writedata(width-1 downto 0);
                    when "010" => iRegPort <= iRegPort and not writedata(width-1 downto 0);
                    when "011" => iRegDir <= writedata(width-1 downto 0);
                    when "101" => iRegIEn <= writedata(width-1 downto 0);
                    when "110" => iRegClrInt <= writedata(width-1 downto 0);
                    when others => null;
                end case;
            end if;
        end if;
    end process pWrite;

    -- Avalon read
    pRead: process(clk)
    begin
        if rising_edge(clk) then
            readdata <= (others => '0');
            if read = '1' then
                case address is
                    when "000" => readdata(width-1 downto 0) <= iRegPort;
                    when "011" => readdata(width-1 downto 0) <= iRegDir;
                    when "100" => readdata(width-1 downto 0) <= iRegPin;
                    when "101" => readdata(width-1 downto 0) <= iRegIEn;
                    when "110" => readdata(width-1 downto 0) <= iRegInt;
                    when others => null;
                end case;
            end if;
        end if;
    end process pRead;

    -- drive the interrupt register
    pInterrupt: process(clk, nReset)
        variable prev: std_logic_vector(width-1 downto 0);
    begin
        if nReset = '0' then
            iRegPinOld <= (others => '0');
            iOldValid <= '0';
            iRegInt <= (others => '0');
        elsif rising_edge(clk) then
            if iOldValid = '1' then
                prev := iRegPinOld;
            else 
                prev := iRegPin;
            end if;
            iRegInt <= (iRegInt and not iRegClrInt) or (iRegPin xor prev);
            iRegPinOld <= iRegPin;
            iOldValid <= '1';
        end if;
    end process pInterrupt;

    -- drive interrupt request
    irq <= '0' when unsigned(iRegInt and iRegIEn) = 0 else '1';

end comp;
