library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package neural_datatypes is

    constant IMAGE_SIZE:    natural := 5;--784; -- 28*28
    constant HIDDEN_SIZE:   natural := 4;--128;
    constant NB_CLASSES:    natural := 2;--10;

    subtype fixed_point is signed(31 downto 0);
    subtype double_fixed_point is signed(63 downto 0);
    type dataflow is array(natural range<>) of fixed_point;
    type matrix is array(natural range<>) of dataflow;

end package neural_datatypes;
