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
    
    result(7 downto 0) <= aData(31 downto 24);
    result(31 downto 24) <= aData(7 downto 0);
    
    result(23) <= aData(8);
    result(22) <= aData(9);
    result(21) <= aData(10);
    result(20) <= aData(11);
    result(19) <= aData(12);
    result(18) <= aData(13);
    result(17) <= aData(14);
    result(16) <= aData(15);
    result(15) <= aData(16);
    result(14) <= aData(17);
    result(13) <= aData(18);
    result(12) <= aData(19);
    result(11) <= aData(20);
    result(10) <= aData(21);
    result(9) <= aData(22);
    result(8) <= aData(23);

end reversal;

