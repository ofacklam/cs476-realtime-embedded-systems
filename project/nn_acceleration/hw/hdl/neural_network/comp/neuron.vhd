-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity Neuron is
    generic(
        in_size:        natural
    );
    port(
        weight:         in dataflow(in_size-1 downto 0);
        --bias:           in fixed_point;
        in_data:        in dataflow(in_size-1 downto 0);
        out_data:       out fixed_point
    );
end Neuron;

architecture comp of Neuron is

    signal before_relu: fixed_point;

begin

    -- compute neuron value
    process(weight, in_data) --(weight, bias, in_data)
        variable tmp: fixed_point;
        variable mult: double_fixed_point;
    begin
        mult := (others => '0');

        -- weight multiplication
        for i in 0 to in_size-1 loop
            mult := mult + weight(i) * in_data(i);
        end loop;
        tmp := mult(23 downto 8);

        -- bias
        --tmp := tmp + bias;

        before_relu <= tmp;
    end process;

    -- relu
    out_data <= before_relu when before_relu >= 0 else (others => '0');

end comp ; -- comp