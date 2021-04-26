library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity op_handler is
    port (
        -- to rest of component
        rdEmpty:        in std_logic;
        read:           out std_logic;
        inData:         in std_logic_vector(31 downto 0);

        wrFull:         in std_logic;
        write:          out std_logic;
        outData:        out std_logic_vector(31 downto 0)
    );
end op_handler;

architecture comp of op_handler is

    signal processing: std_logic;

    component combination is
        port(
            -- Avalon interfaces signals
            aData : in std_logic_vector (31 downto 0);
            bData : in std_logic_vector (31 downto 0);
            result: out std_logic_vector (31 downto 0)
        );
    end component combination;

begin

    processing <= (not rdEmpty) and (not wrFull);
    read <= processing;
    write <= processing;

    op: component combination
        port map (
            aData => inData,
            bData => X"00000000",
            result => outData
        );

end comp;
