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
        bias:           in fixed_point;
        in_data:        in dataflow(in_size-1 downto 0);
        out_data:       out fixed_point
    );
end Neuron;

architecture comp of Neuron is
begin

    -- compute neuron value
    process(weight, bias, in_data)
        variable tmp: fixed_point;
        variable mult: double_fixed_point;
    begin
        mult := (others => '0');

        -- weight multiplication
        for i in 0 to in_size-1 loop
            mult := mult + weight(i) * in_data(i);
        end loop;
        tmp := mult(47 downto 16);

        -- bias
        tmp := tmp + bias;

        -- relu
        out_data <= tmp when tmp >= 0 else (others => '0');
    end process;

end comp ; -- comp