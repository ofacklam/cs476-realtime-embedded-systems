library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity ParallelPort is
    port (
        -- Avalon signals
        clk:        in std_logic;
        nReset:     in std_logic;

        address:    in std_logic_vector(2 downto 0);

        read:       in std_logic;
        readdata:   out std_logic_vector(7 downto 0);

        write:      in std_logic;
        writedata:  in std_logic_vector(7 downto 0);

        -- Conduit
        ParPort:    inout std_logic_vector(7 downto 0)
    );
end ParallelPort;


architecture comp of ParallelPort is

    -- internal signals
    signal iRegDir:         std_logic_vector(7 downto 0);
    signal iRegPort:        std_logic_vector(7 downto 0);
    signal iRegPin:         std_logic_vector(7 downto 0);

begin

    -- drive the conduit signal
    pPort: process(iRegDir, iRegPort)
    begin
        for i in 0 to 7 loop
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
        elsif rising_edge(clk) then
            if write = '1' then
                case address is
                    when "000" => iRegDir <= writedata;
                    when "010" => iRegPort <= writedata;
                    when "011" => iRegPort <= iRegPort or writedata;
                    when "100" => iRegPort <= iRegPort and not writedata;
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
                    when "000" => readdata <= iRegDir;
                    when "001" => readdata <= iRegPin;
                    when "010" => readdata <= iRegPort;
                    when others => null;
                end case;
            end if;
        end if;
    end process pRead;

end comp;
