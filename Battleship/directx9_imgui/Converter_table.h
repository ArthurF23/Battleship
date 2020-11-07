#pragma once
#include <string>
using namespace std;
namespace std {

	class Convert {

	public:
		static string ASCII_TO_BIN(char input);

		static char BIN_TO_ASCII(string inp);		

		static string HEX_TO_BINARY(char inp);

		static char BINARY_TO_HEX(string inp);
	};
}