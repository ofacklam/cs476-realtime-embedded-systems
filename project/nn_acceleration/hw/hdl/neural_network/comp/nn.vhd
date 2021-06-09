library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity Network is
    port(
        weight0:        in matrix(HIDDEN_SIZE-1 downto 0)(IMAGE_SIZE-1 downto 0);
        bias0:          in dataflow(HIDDEN_SIZE-1 downto 0);

        weight1:        in matrix(HIDDEN_SIZE-1 downto 0)(HIDDEN_SIZE-1 downto 0);
        bias1:          in dataflow(HIDDEN_SIZE-1 downto 0);
        
        weight2:        in matrix(NB_CLASSES-1 downto 0)(HIDDEN_SIZE-1 downto 0);
        bias2:          in dataflow(NB_CLASSES-1 downto 0);
        
        in_data:        in dataflow(IMAGE_SIZE-1 downto 0);
        out_data:       out dataflow(NB_CLASSES-1 downto 0)
    );
end Network;

architecture comp of Network is

    signal hidden1:     dataflow(HIDDEN_SIZE-1 downto 0);
    signal hidden2:     dataflow(HIDDEN_SIZE-1 downto 0);

    component Layer is
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
    end component;

begin

    layer0: Layer
    generic map(
        in_size => IMAGE_SIZE,
        out_size => HIDDEN_SIZE
    )
    port map(
        weight => weight0,
        bias => bias0,
        in_data => in_data,
        out_data => hidden1
    );

    layer1: Layer
    generic map(
        in_size => HIDDEN_SIZE,
        out_size => HIDDEN_SIZE
    )
    port map(
        weight => weight1,
        bias => bias1,
        in_data => hidden1,
        out_data => hidden2
    );

    layer2: Layer
    generic map(
        in_size => HIDDEN_SIZE,
        out_size => NB_CLASSES
    )
    port map(
        weight => weight2,
        bias => bias2,
        in_data => hidden2,
        out_data => out_data
    );

end comp ; -- comp