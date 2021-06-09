library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Registers is
    port(
        nReset:         in std_logic;
        clk:            in std_logic;

        -- Avalon slave
        AS_address:     in std_logic_vector(1 downto 0);
        AS_read:        in std_logic;
        AS_readdata:    out std_logic_vector(31 downto 0) := (others => '0');
        AS_write:       in std_logic;
        AS_writedata:   in std_logic_vector(31 downto 0);

        -- connection to other controller modules
        buf0Address:    buffer std_logic_vector(31 downto 0) := (others => '0');
        bufLength:      buffer std_logic_vector(31 downto 0) := (others => '0');
        bufNumber:      buffer unsigned(2 downto 0) := (others => '0');
        start:          buffer std_logic := '0';
        busy:           in std_logic
    );
end Registers;

architecture comp of Registers is
begin

    -- Avalon slave write
    process(clk, nReset)
    begin
        if nReset = '0' then
            buf0Address <= (others => '0');
            bufLength <= (others => '0');
            bufNumber <= (others => '0');
            start <= '0';
        elsif rising_edge(clk) then
            start <= '0';
            if AS_write = '1' then
                case AS_address is
                    when "00" => buf0Address <= AS_writedata;
                    when "01" => bufLength <= "000" & AS_writedata(28 downto 0);
                                bufNumber <= unsigned(AS_writedata(31 downto 29));
                    when "10" => start <= '1';
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
                    when "00" => AS_readdata <= buf0Address;
                    when "01" => AS_readdata <= std_logic_vector(bufNumber) & bufLength(28 downto 0);
                    when "11" => AS_readdata(0) <= busy;
                    when others => null;
                end case;
            end if;
        end if;
    end process;

end comp ; -- comp