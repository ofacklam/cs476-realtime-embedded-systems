-- altera vhdl_input_version vhdl_2008

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity Argmax is
    generic(
        size:           natural
    );
    port(
        in_data:        in dataflow(size-1 downto 0);
        out_data:       out std_logic_vector(size-1 downto 0)
    );
end Argmax;

architecture comp of Argmax is
begin

    -- compute the argmax
    process(in_data)
        variable max_val: fixed_point := X"0000";
        variable max_idx: natural := 0;
    begin
        
        for i in 0 to size-1 loop
            if in_data(i) >= max_val then
                max_val := in_data(i);
                max_idx := i;
            end if;
        end loop;
        
        out_data <= (others => '0');
        out_data(max_idx) <= '1';
    end process;

end comp ; -- comp