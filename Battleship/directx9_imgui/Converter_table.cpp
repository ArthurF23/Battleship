#include <string>
using namespace std;
#include "Converter_table.h"

namespace std {

	string Convert::ASCII_TO_BIN(char input) {
		static string bin_str;
		switch (input) {

		case 'A':
			bin_str = "01000001";
			break;

		case 'B':
			bin_str = "01000010";
			break;

		case 'C':
			bin_str = "01000011";
			break;

		case 'D':
			bin_str = "01000100";
			break;

		case 'E':
			bin_str = "01000101";
			break;

		case 'F':
			bin_str = "01000110";
			break;

		case 'G':
			bin_str = "01000111";
			break;

		case 'H':
			bin_str = "01001000";
			break;

		case 'I':
			bin_str = "01001001";
			break;

		case 'J':
			bin_str = "01001010";
			break;

		case 'K':
			bin_str = "01001011";
			break;

		case 'L':
			bin_str = "01001100";
			break;

		case 'M':
			bin_str = "01001101";
			break;

		case 'N':
			bin_str = "01001110";
			break;

		case 'O':
			bin_str = "01001111";
			break;

		case 'P':
			bin_str = "01010000";
			break;

		case 'Q':
			bin_str = "01010001";
			break;

		case 'R':
			bin_str = "01010010";
			break;

		case 'S':
			bin_str = "01010011";
			break;

		case 'T':
			bin_str = "01010100";
			break;

		case 'U':
			bin_str = "01010101";
			break;

		case 'V':
			bin_str = "01010110";
			break;

		case 'W':
			bin_str = "01010111";
			break;

		case 'X':
			bin_str = "01011000";
			break;

		case 'Y':
			bin_str = "01011001";
			break;

		case 'Z':
			bin_str = "01011010";
			break;

		case 'a':
			bin_str = "01100001";
			break;

		case 'b':
			bin_str = "01100010";
			break;

		case 'c':
			bin_str = "01100011";
			break;

		case 'd':
			bin_str = "01100100";
			break;

		case 'e':
			bin_str = "01100101";
			break;

		case 'f':
			bin_str = "01100110";
			break;

		case 'g':
			bin_str = "01100111";
			break;

		case 'h':
			bin_str = "01101000";
			break;

		case 'i':
			bin_str = "01101001";
			break;

		case 'j':
			bin_str = "01101010";
			break;

		case 'k':
			bin_str = "01101011";
			break;

		case 'l':
			bin_str = "01101100";
			break;

		case 'm':
			bin_str = "01101101";
			break;

		case 'n':
			bin_str = "01101110";
			break;

		case 'o':
			bin_str = "01101111";
			break;

		case 'p':
			bin_str = "01110000";
			break;

		case 'q':
			bin_str = "01110001";
			break;

		case 'r':
			bin_str = "01110010";
			break;

		case 's':
			bin_str = "01110011";
			break;

		case 't':
			bin_str = "01110100";
			break;

		case 'u':
			bin_str = "01110101";
			break;

		case 'v':
			bin_str = "01110110";
			break;

		case 'w':
			bin_str = "01110111";
			break;

		case 'x':
			bin_str = "01111000";
			break;

		case 'y':
			bin_str = "01111001";
			break;

		case 'z':
			bin_str = "01111010";
			break;

		case ' ':
			bin_str = "00100000";
			break;

		case '!':
			bin_str = "00100001";
			break;

		case '?':
			bin_str = "00111111";
			break;

		case ',':
			bin_str = "00101100";
			break;

		case '\'':
			bin_str = "00100111";
			break;

		case '"':
			bin_str = "00100010";
			break;

		case '.':
			bin_str = "00101110";
			break;

		case ';':
			bin_str = "00111011";
			break;

		case '0':
			bin_str = "00110000";
			break;

		case '1':
			bin_str = "00110001";
			break;

		case '2':
			bin_str = "00110010";
			break;

		case '3':
			bin_str = "00110011";
			break;

		case '4':
			bin_str = "00110100";
			break;

		case '5':
			bin_str = "00110101";
			break;

		case '6':
			bin_str = "00110110";
			break;

		case '7':
			bin_str = "00110111";
			break;

		case '8':
			bin_str = "00111000";
			break;

		case '9':
			bin_str = "00111001";
			break;

		default:
			bin_str = "";
			break;
		}
		return bin_str;
	}
	

	char Convert::BINARY_TO_HEX(string inp) {
		static char hex_bin;
		if (inp == "0000") { hex_bin = '0'; }
		else if (inp == "0001") { hex_bin = '1'; }
		else if (inp == "0010") { hex_bin = '2'; }
		else if (inp == "0011") { hex_bin = '3'; }
		else if (inp == "0100") { hex_bin = '4'; }
		else if (inp == "0101") { hex_bin = '5'; }
		else if (inp == "0110") { hex_bin = '6'; }
		else if (inp == "0111") { hex_bin = '7'; }
		else if (inp == "1000") { hex_bin = '8'; }
		else if (inp == "1001") { hex_bin = '9'; }

		else if (inp == "1010") { hex_bin = 'A'; }
		else if (inp == "1011") { hex_bin = 'B'; }
		else if (inp == "1100") { hex_bin = 'C'; }
		else if (inp == "1101") { hex_bin = 'D'; }
		else if (inp == "1110") { hex_bin = 'E'; }
		else if (inp == "1111") { hex_bin = 'F'; }
		return hex_bin;
	}


	string Convert::HEX_TO_BINARY(char inp) {
		static string bin_val;
		switch (inp) {

		case '0':
			bin_val = "0000";
			break;

		case '1':
			bin_val = "0001";
			break;

		case '2':
			bin_val = "0010";
			break;

		case '3':
			bin_val = "0011";
			break;
			
		case '4':
			bin_val = "0100";
			break;

		case '5':
			bin_val = "0101";
			break;

		case '6':
			bin_val = "0110";
			break;

		case '7':
			bin_val = "0111";
			break;

		case '8':
			bin_val = "1000";
			break;

		case '9':
			bin_val = "1001";
			break;

		case 'A':
			bin_val = "1010";
			break;

		case 'B':
			bin_val = "1011";
			break;

		case 'C':
			bin_val = "1100";
			break;

		case 'D':
			bin_val = "1101";
			break;

		case 'E':
			bin_val = "1110";
			break;

		case 'F':
			bin_val = "1111";
			break;

		case ' ':
			bin_val = "";
			break;

		default:
			break;
		}		
		return bin_val;
	}

	char Convert::BIN_TO_ASCII(string inp) {
		static char placeholder;

		if (inp == "01000001") { placeholder = 'A'; }
		else if (inp == "01000010") { placeholder = 'B'; }
		else if (inp == "01000011") { placeholder = 'C'; }
		else if (inp == "01000100") { placeholder = 'D'; }
		else if (inp == "01000101") { placeholder = 'E'; }
		else if (inp == "01000110") { placeholder = 'F'; }
		else if (inp == "01000111") { placeholder = 'G'; }
		else if (inp == "01001000") { placeholder = 'H'; }
		else if (inp == "01001001") { placeholder = 'I'; }
		else if (inp == "01001010") { placeholder = 'J'; }
		else if (inp == "01001011") { placeholder = 'K'; }
		else if (inp == "01001100") { placeholder = 'L'; }
		else if (inp == "01001101") { placeholder = 'M'; }
		else if (inp == "01001110") { placeholder = 'N'; }
		else if (inp == "01001111") { placeholder = 'O'; }
		else if (inp == "01010000") { placeholder = 'P'; }
		else if (inp == "01010001") { placeholder = 'Q'; }
		else if (inp == "01010010") { placeholder = 'R'; }
		else if (inp == "01010011") { placeholder = 'S'; }
		else if (inp == "01010100") { placeholder = 'T'; }
		else if (inp == "01010101") { placeholder = 'U'; }
		else if (inp == "01010110") { placeholder = 'V'; }
		else if (inp == "01010111") { placeholder = 'W'; }
		else if (inp == "01011000") { placeholder = 'X'; }
		else if (inp == "01011001") { placeholder = 'Y'; }
		else if (inp == "01011010") { placeholder = 'Z'; }

		else if (inp == "01100001") { placeholder = 'a'; }
		else if (inp == "01100010") { placeholder = 'b'; }
		else if (inp == "01100011") { placeholder = 'c'; }
		else if (inp == "01100100") { placeholder = 'd'; }
		else if (inp == "01100101") { placeholder = 'e'; }
		else if (inp == "01100110") { placeholder = 'f'; }
		else if (inp == "01100111") { placeholder = 'g'; }
		else if (inp == "01101000") { placeholder = 'h'; }
		else if (inp == "01101001") { placeholder = 'i'; }
		else if (inp == "01101010") { placeholder = 'j'; }
		else if (inp == "01101011") { placeholder = 'k'; }
		else if (inp == "01101100") { placeholder = 'l'; }
		else if (inp == "01101101") { placeholder = 'm'; }
		else if (inp == "01101110") { placeholder = 'n'; }
		else if (inp == "01101111") { placeholder = 'o'; }
		else if (inp == "01110000") { placeholder = 'p'; }
		else if (inp == "01110001") { placeholder = 'q'; }
		else if (inp == "01110010") { placeholder = 'r'; }
		else if (inp == "01110011") { placeholder = 's'; }
		else if (inp == "01110100") { placeholder = 't'; }
		else if (inp == "01110101") { placeholder = 'u'; }
		else if (inp == "01110110") { placeholder = 'v'; }
		else if (inp == "01110111") { placeholder = 'w'; }
		else if (inp == "01111000") { placeholder = 'x'; }
		else if (inp == "01111001") { placeholder = 'y'; }
		else if (inp == "01111010") { placeholder = 'z'; }

		else if (inp == "00100000") { placeholder = ' '; }

		else if (inp == "00100001") { placeholder = '!'; }
		else if (inp == "00111111") { placeholder = '?'; }
		else if (inp == "00101100") { placeholder = ','; }
		else if (inp == "00100010") { placeholder = '"'; }
		else if (inp == "00100111") { placeholder = '\''; }
		else if (inp == "00101110") { placeholder = '.'; }
		else if (inp == "00111011") { placeholder = ';'; }


		else if (inp == "00110000") { placeholder = '0'; }
		else if (inp == "00110001") { placeholder = '1'; }
		else if (inp == "00110010") { placeholder = '2'; }
		else if (inp == "00110011") { placeholder = '3'; }
		else if (inp == "00110100") { placeholder = '4'; }
		else if (inp == "00110101") { placeholder = '5'; }
		else if (inp == "00110110") { placeholder = '6'; }
		else if (inp == "00110111") { placeholder = '7'; }
		else if (inp == "00111000") { placeholder = '8'; }
		else if (inp == "00111001") { placeholder = '9'; }

		return placeholder;
	}
}