library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neural_datatypes.all;

entity tb_nn is
end tb_nn;

architecture test of tb_nn is

    -- random test weights
    constant W0: matrix(HIDDEN_SIZE-1 downto 0)(IMAGE_SIZE-1 downto 0)
        := (
            0 => (0 => 32D"63855", 1 => 32D"1180", 2 => 32D"50737", 3 => 32D"64131", 4 => 32D"42415"),
            1 => (0 => 32D"52502", 1 => 32D"1738", 2 => 32D"4773", 3 => 32D"1924", 4 => 32D"26725"),
            2 => (0 => 32D"26322", 1 => 32D"54593", 2 => 32D"7084", 3 => 32D"32276", 4=> 32D"30054"),
            3 => (0 => 32D"28819", 1 => 32D"59274", 2 => 32D"64794", 3 => 32D"63178", 4 => 32D"36052")
        );
    constant B0: dataflow(HIDDEN_SIZE-1 downto 0)
        := (0 => 32D"37422", 1 => 32D"20130", 2 => 32D"25557", 3 => 32D"36765");
    constant W1: matrix(HIDDEN_SIZE-1 downto 0)(HIDDEN_SIZE-1 downto 0)
        := (
            0 => (0 => 32D"65019", 1 => 32D"48585", 2 => 32D"54776", 3 => 32D"8773"),
            1 => (0 => 32D"5928", 1 => 32D"60332", 2 => 32D"37258", 3 => 32D"8847"),
            2 => (0 => 32D"12628", 1 => 32D"30720", 2 => 32D"15542", 3 => 32D"33682"),
            3 => (0 => 32D"16200", 1 => 32D"60921", 2 => 32D"42312", 3 => 32D"64631")
        );
    constant B1: dataflow(HIDDEN_SIZE-1 downto 0)
        := (0 => 32D"62936", 1 => 32D"48400", 2 => 32D"1809", 3 => 32D"56557");
    constant W2: matrix(NB_CLASSES-1 downto 0)(HIDDEN_SIZE-1 downto 0)
        := (
            0 => (0 => 32D"38802", 1 => 32D"43725", 2 => 32D"16713", 3 => 32D"62246"),
            1 => (0 => 32D"64465", 1 => 32D"25535", 2 => 32D"57793", 3 => 32D"46705")
        );
    constant B2: dataflow(NB_CLASSES-1 downto 0)
        := (0 => 32D"38426", 1 => 32D"46391");

    -- random test vectors
    constant X1: dataflow(IMAGE_SIZE-1 downto 0)
        := (0 => 32D"1760", 1 => 32D"58972", 2 => 32D"63653", 3 => 32D"23625", 4 => 32D"54270");
    constant X2: dataflow(IMAGE_SIZE-1 downto 0)
        := (0 => 32D"11673", 1 => 32D"2278", 2 => 32D"27188", 3 => 32D"38067", 4 => 32D"52444");

    -- expected results
    constant Y1: dataflow(NB_CLASSES-1 downto 0)
        := (0 => 32D"842976", 1 => 32D"959128");
    constant Y2: dataflow(NB_CLASSES-1 downto 0)
        := (0 => 32D"693914", 1 => 32D"793628");

    -- neural net signals
    signal in_data:        dataflow(IMAGE_SIZE-1 downto 0);
    signal out_data:       dataflow(NB_CLASSES-1 downto 0);

begin

    -- instantiate the cam interface
    dut: entity work.Network
    port map(
        weight0 => W0,
        bias0 => B0,
        weight1 => W1,
        bias1 => B1,
        weight2 => W2,
        bias2 => B2,
        in_data => in_data,
        out_data => out_data
    );

    -- test the cam interface
    simulation: process

            procedure checkValue(actual: in fixed_point; 
                                expected: in fixed_point) is
            begin
                assert actual = expected
                report "Unexpected result: " &
                        "Read = " & integer'image(to_integer(unsigned(actual))) &
                        "Expected = " & integer'image(to_integer(unsigned(expected)))
                severity error;
            end procedure;

            procedure checkResult(actual: in dataflow(NB_CLASSES-1 downto 0);
                                    expected: in dataflow(NB_CLASSES-1 downto 0)) is
            begin

                for i in 0 to NB_CLASSES-1 loop
                    checkValue(actual(i), expected(i));
                end loop;
                
            end procedure;

    begin

        -- test vector 1
        in_data <= X1;
        wait for 100 ns;
        checkResult(out_data, Y1);

        -- test vector 2
        in_data <= X2;
        wait for 100 ns;
        checkResult(out_data, Y2);

        wait;

    end process;

end architecture test;