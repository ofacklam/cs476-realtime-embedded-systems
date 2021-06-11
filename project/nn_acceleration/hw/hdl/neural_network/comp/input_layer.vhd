-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity InputLayer is
    generic(
        out_size:        natural
    );
    port(
        clk:            in std_logic;
        reset_acc:      in std_logic;

        weight:         in dataflow(out_size-1 downto 0);
        --bias:           in dataflow(out_size-1 downto 0);
        in_data:        in fixed_point;
        out_data:       out dataflow(out_size-1 downto 0);

        accumulate:     in std_logic
    );
end InputLayer;

architecture comp of InputLayer is

    signal before_relu: dataflow(out_size-1 downto 0);

begin

    -- accumulate input layer value
    process(clk, reset_acc)
        variable mult: double_fixed_point;
    begin
        if reset_acc = '1' then
            before_relu <= (others => (others => '0')); --bias;
        elsif rising_edge(clk) then
            if accumulate = '1' then
                
                for i in 0 to out_size-1 loop
                    mult := weight(i) * in_data;
                    before_relu(i) <= before_relu(i) + mult(23 downto 8);
                end loop;

            end if;
        end if;
    end process;

    -- relu
    process(before_relu)
    begin
        for i in 0 to out_size-1 loop
            if before_relu(i) >= 0 then
                out_data(i) <= before_relu(i);
            else
                out_data(i) <= (others => '0');
            end if;
        end loop;
    end process;

end comp ; -- comp