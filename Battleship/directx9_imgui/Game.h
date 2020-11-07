#pragma once
#include <string>
using namespace std;
namespace std {

	class SERVER {
	public:
		static int START();

		static int SEND(string msg);

		static bool CONNECTED_TO_CLIENT;
	};

	class CLIENT {
	public:
		static int START(string ip);

		static int SEND(string ip);

		static bool CONNECTED_TO_SERVER;
	};

	class GAME {
	public:
		enum TURN {
			P1,
			P2
		};
	};

}