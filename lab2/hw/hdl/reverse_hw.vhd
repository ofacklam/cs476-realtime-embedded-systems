library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

ENTITY combination IS
PORT(
-- Avalon interfaces signals

aData : IN std_logic_vector (31 DOWNTO 0);
bData : IN std_logic_vector (31 DOWNTO 0);
result: OUT std_logic_vector (31 DOWNTO 0)

);
End combination;

architecture reversal of combination is
--internal signals

begin
-- reverse process
    reverse_bits: process(aData)
    begin
	    result(7 downto 0) <= aData(31 downto 24);
	    result(31 downto 24) <= aData(7 downto 0);
	    result(8 to 23) <= aData(23 downto 8);
   end process reverse_bits;
-- Process Write to registers

end reversal;

