library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity UART is
	port (BUTTON : in std_logic;
			LEDS : out std_logic_vector(3 downto 0);
			CLK27M : in std_logic;
			TX : out std_logic;
			RX : in std_logic;
			canal_A: in std_logic;
			canal_B: in std_logic;
			reset: in std_logic);
end UART;

architecture comportamento of UART is
signal clkRX, clkTX,recebendo: std_logic;
signal cont, cont2: std_logic_vector(23 downto 0);
signal cont160: std_logic_vector(7 downto 0);
signal prox, atual, palavra: std_logic_vector(9 downto 0);
signal DISPW : std_logic_vector(3 downto 0);
signal rot_event : std_logic;

begin
	
	process(canal_A)
	begin
		if(canal_A'event and canal_A='1') then
--				if(rot_event = '1') then
--					rot_event<='0';
--				else
--					rot_event<='1';
				if(canal_A=canal_B) then
						DISPW <= "0100";
				else
					DISPW <= "1000";
				end if;
--				end if;
		end if;
	end process;
	

	process(CLK27M)
	begin
		if(CLK27M'event and CLK27M = '1') then
			cont <= cont + "000000000000000000000001";
			cont2 <= cont2 + "000000000000000000000001";
			if(cont = "000000000000101011111100") then
				cont <= "000000000000000000000000";
			end if;
			if(cont2 = "000000000000000010110000") then
				cont2 <= "000000000000000000000000";
			end if;
		end if;
	end process;
		
	clkTX <= cont(11); -- clkTX tem 9600 Hz
	clkRX <= cont2(7); -- clkRX tem 9600 x 16 Hz
	prox <= "10" & DISPW & "1100" when (canal_A='1' and canal_B='1') else
	atual (8 downto 0) & '1';
	TX <= atual(9);

	process(clkTX)
		begin
			if (clkTX'event and clkTX = '1') then
				atual <= prox;
			end if;
	end process;

	process(clkRX)
		begin
			if (clkRX'event and clkRX = '1') then
				if(recebendo = '0') then
					if(RX = '1') then
						cont160 <= "00000000";
					else
						recebendo <= '1'; -- deteco do START BIT
					end if;
				end if;
				if(recebendo = '1') then
					cont160 <= cont160 + "00000001";
					if(cont160(3 downto 0) = "1000") then
						palavra <= RX & palavra (9 downto 1);
					end if;
					if (cont160 = "10011100") then
						recebendo <= '0';
						LEDS <= palavra (4 downto 1);
					end if;
				end if;
			end if;
	end process;
end comportamento;
