library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity Layer is
    generic(
        in_size:        natural;
        out_size:       natural
    );
    port(
        weight:         in matrix(out_size-1 downto 0)(in_size-1 downto 0);
        bias:           in dataflow(out_size-1 downto 0);
        in_data:        in dataflow(in_size-1 downto 0);
        out_data:       out dataflow(out_size-1 downto 0)
    );
end Layer;

architecture comp of Layer is

    component Neuron is
        generic(
            in_size:        natural
        );
        port(
            weight:         in dataflow(in_size-1 downto 0);
            bias:           in fixed_point;
            in_data:        in dataflow(in_size-1 downto 0);
            out_data:       out fixed_point
        );
    end component;

begin

    gen_neurons:
    for i in 0 to out_size-1 generate
        n: Neuron
        generic map(
            in_size => in_size
        )
        port map (
            weight => weight(i),
            bias => bias(i),
            in_data => in_data,
            out_data => out_data(i)
        );
    end generate;

end comp ; -- comp