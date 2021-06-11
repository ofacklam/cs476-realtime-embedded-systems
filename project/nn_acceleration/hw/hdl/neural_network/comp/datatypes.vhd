-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package neural_datatypes is

    constant IMAGE_SIZE:    natural := 256; -- 16*16
    constant HIDDEN_SIZE:   natural := 16;
    constant NB_CLASSES:    natural := 5;

    subtype fixed_point is signed(15 downto 0);
    subtype double_fixed_point is signed(31 downto 0);
    type dataflow is array(natural range<>) of fixed_point;
    type matrix is array(natural range<>) of dataflow;

end package neural_datatypes;
