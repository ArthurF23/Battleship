#pragma once
#include <string>
using namespace std;
namespace std {

	class SERVER {
	public:
		static int START();

		static int SEND(string msg);

		static bool CONNECTED_TO_CLIENT;

		static bool IS_STARTED;

		static string RECENTMESSAGE;
	};

	class CLIENT {
	public:
		static int START(string ip);

		static int SEND(string ip);

		static bool CONNECTED_TO_SERVER;

		static bool IS_STARTED;

		static string RECENTMESSAGE;
	};

	class GAME {
	public:
		enum TURN {
			P1,
			P2
		};
	};

}