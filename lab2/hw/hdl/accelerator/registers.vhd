library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity registers is
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
end registers;

architecture comp of registers is

begin

    -- Avalon read (1-wait)
    pRegRead: process(clk)
    begin
        if rising_edge(clk) then
            readdata <= (others => '0');
            if read = '1' then
                case address is
                    when "00" => readdata <= srcAddr;
                    when "01" => readdata <= dstAddr;
                    when "10" => readdata <= std_logic_vector(len);
                end case;
            end if;
        end if;
    end process pRegRead;

    -- Avalon write (0-wait)
    pRegWrite: process(clk, nReset)
    begin
        if nReset = '0' then
            srcAddr <= (others => '0');
            dstAddr <= (others => '0');
            len <= (others => '0');
        elsif rising_edge(clk) then
            if write = '1' then
                case address is
                    when "00" => srcAddr <= writedata;
                    when "01" => dstAddr <= writedata;
                    when "10" => len <= unsigned(writedata);
                end case;
            end if;
        end if;
    end process pRegWrite;

end comp;
