#include <iostream>
#include <atomic>
#include <sstream>
using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32 // for Windows systems

#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#else // for *nix systems

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

#endif
struct IPv4
{
	unsigned char b1, b2, b3, b4;

	string to_str() {
		string s = to_string(b1) + "." + to_string(b2) + "." + to_string(b3) + "." + to_string(b4);
		
		return s;
	}
};

bool getMyIP(IPv4* myIP)
{
	char szBuffer[1024];

#ifdef WIN32
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	if (::WSAStartup(wVersionRequested, &wsaData) != 0)
		return false;
#endif


	if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}

	struct hostent* host = gethostbyname(szBuffer);
	if (host == NULL)
	{
#ifdef WIN32
		WSACleanup();
#endif
		return false;
	}

	//Obtain the computer's IP
	myIP->b1 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1;
	myIP->b2 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2;
	myIP->b3 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3;
	myIP->b4 = ((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
	WSACleanup();
#endif
	return true;
}
SOCKET clientSocket;
SOCKET sock;
bool namerecived = false;
#include "Game.h"
namespace std {
	bool SERVER::IS_STARTED = false;

	bool CLIENT::IS_STARTED = false;

	string CLIENT::NAME = "";

	string SERVER::RECENTMESSAGE = "";

	string CLIENT::RECENTMESSAGE = "";

	bool SERVER::CONNECTED_TO_CLIENT = false;

	bool CLIENT::CONNECTED_TO_SERVER = false;

	int GAME::POS_TO_ARR(string POS) {
		char LETTER = POS.at(0); //Get the Letter like "C" of C-7
		int NUM = stoi(POS.substr(2, POS.length())); //Get the Number like "7" of C-7

		switch (LETTER) {
		case 'A':
			switch (NUM) {
			case 1:
				return 12;

			case 2:
				return 13;

			case 3:
				return 14;

			case 4:
				return 15;

			case 5:
				return 16;

			case 6:
				return 17;

			case 7:
				return 18;

			case 8:
				return 19;

			case 9:
				return 20;

			case 10:
				return 21;

			default:
				return -1;
			};
			break;

		case 'B':
			switch (NUM) {
			case 1:
				return 23;

			case 2:
				return 24;

			case 3:
				return 25;

			case 4:
				return 26;

			case 5:
				return 27;

			case 6:
				return 28;

			case 7:
				return 29;

			case 8:
				return 30;

			case 9:
				return 31;

			case 10:
				return 32;

			default:
				return -1;
			};
			break;

		case 'C':
			switch (NUM) {
			case 1:
				return 34;

			case 2:
				return 35;

			case 3:
				return 36;

			case 4:
				return 37;

			case 5:
				return 38;

			case 6:
				return 39;

			case 7:
				return 40;

			case 8:
				return 41;

			case 9:
				return 42;

			case 10:
				return 43;

			default:
				return -1;
			};
			break;

		case 'D':
			switch (NUM) {
			case 1:
				return 45;

			case 2:
				return 46;

			case 3:
				return 47;

			case 4:
				return 48;

			case 5:
				return 49;

			case 6:
				return 50;

			case 7:
				return 51;

			case 8:
				return 52;

			case 9:
				return 53;

			case 10:
				return 54;

			default:
				return -1;
			};
			break;

		case 'E':
			switch (NUM) {
			case 1:
				return 56;

			case 2:
				return 57;

			case 3:
				return 58;

			case 4:
				return 59;

			case 5:
				return 60;

			case 6:
				return 61;

			case 7:
				return 62;

			case 8:
				return 63;

			case 9:
				return 64;

			case 10:
				return 65;

			default:
				return -1;
			};
			break;

		case 'F':
			switch (NUM) {
			case 1:
				return 67;

			case 2:
				return 68;

			case 3:
				return 69;

			case 4:
				return 70;

			case 5:
				return 71;

			case 6:
				return 72;

			case 7:
				return 73;

			case 8:
				return 74;

			case 9:
				return 75;

			case 10:
				return 76;

			default:
				return -1;
			};
			break;

		case 'G':
			switch (NUM) {
			case 1:
				return 78;

			case 2:
				return 79;

			case 3:
				return 80;

			case 4:
				return 81;

			case 5:
				return 82;

			case 6:
				return 83;

			case 7:
				return 84;

			case 8:
				return 85;

			case 9:
				return 86;

			case 10:
				return 87;

			default:
				return -1;
			};
			break;

		case 'H':
			switch (NUM) {
			case 1:
				return 89;

			case 2:
				return 90;

			case 3:
				return 91;

			case 4:
				return 92;

			case 5:
				return 93;

			case 6:
				return 94;

			case 7:
				return 95;

			case 8:
				return 96;

			case 9:
				return 97;

			case 10:
				return 98;

			default:
				return -1;
			};
			break;

		case 'I':
			switch (NUM) {
			case 1:
				return 100;

			case 2:
				return 101;

			case 3:
				return 102;

			case 4:
				return 103;

			case 5:
				return 104;

			case 6:
				return 105;

			case 7:
				return 106;

			case 8:
				return 107;

			case 9:
				return 108;

			case 10:
				return 109;

			default:
				return -1;
			};
			break;

		case 'J':
			switch (NUM) {
			case 1:
				return 111;

			case 2:
				return 112;

			case 3:
				return 113;

			case 4:
				return 114;

			case 5:
				return 115;

			case 6:
				return 116;

			case 7:
				return 117;

			case 8:
				return 118;

			case 9:
				return 119;

			case 10:
				return 120;

			default:
				return -1;
			};
			break;
		default:
			return -1;
		};

	};

	string SERVER::GET_IP() {
		IPv4* ip = new IPv4;
		getMyIP(ip);
		return ip->to_str();
	};

	int SERVER::START() {
		// Initialize winsock
#ifdef _WIN32
	// Initialize winsock
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);

		int wsOk = WSAStartup(ver, &wsData);
		if (wsOk != 0)
		{
			std::cerr << "Can't Initialize winsock! Quitting..." << '\n';
			return -1;
		}
#endif

		// Create a socket
#ifdef _WIN32
		SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening == INVALID_SOCKET)
		{
			std::cerr << "Can't create a socket! Quitting..." << '\n';
			return -1;
		}

#else
		int listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening == -1)
		{
			std::cerr << "Can't create a socket! Quitting..." << '\n';
			return -1;
		}

#endif

		// Bind ip address and port to a socket
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(54000); // host to network shot

#ifdef _WIN32
		hint.sin_addr.S_un.S_addr = INADDR_ANY;		
		bind(listening, (sockaddr*)&hint, sizeof(hint));

#else
		inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
		if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
		{
			std::cerr << "Can't bind to IP/port\n";
			return -2;
		}

#endif

		// Tell winsock the socket is for listening
#ifdef _WIN32
		listen(listening, SOMAXCONN);

#else
		if (listen(listening, SOMAXCONN) == -1)
		{
			std::cerr << "Can't listen!\n";
			return -3;
		}

#endif

		// Wait for a connection		
		sockaddr_in client;
#ifdef _WIN32
		int clientSize = sizeof(client);
		
		clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << "Invalid socket name! Quitting..." << '\n';
			return -4;
		}

		char host[NI_MAXHOST];		// Clients remote name
		char service[NI_MAXSERV];	// Service (i.e. port) the client is connected on

		ZeroMemory(host, NI_MAXHOST); // same as (unix-system) memset(host, 0, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			std::cout << host << " connected on port " << service << '\n';
			SERVER::CONNECTED_TO_CLIENT = true;
		}
		else
		{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on port" << ntohs(client.sin_port) << '\n';
			SERVER::CONNECTED_TO_CLIENT = true;
		}

#else
		socklen_t clientSize = sizeof(client);
		char host[NI_MAXHOST];
		char svc[NI_MAXSERV];
		int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

		if (clientSocket == -1)
		{
			std::cerr << "Problem with client connecting!\n";
			return -4;
		}

		memset(host, 0, NI_MAXHOST);
		memset(svc, 0, NI_MAXSERV);

		int result = getnameinfo((sockaddr*)&client, sizeof(client),
			host, NI_MAXHOST, svc, NI_MAXSERV, 0);

		if (result)
		{
			std::cout << host << " connected on " << svc << '\n';
		}
		else
		{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on " << ntohs(client.sin_port) << '\n';
		}

#endif

		// Close the listening socket
#ifdef _WIN32
		closesocket(listening);

#else
		close(listening);

#endif

		// While loop: accept and echo message back to client
		char buf[4096] = "";
#ifdef _WIN32
		while (true)
		{
			ZeroMemory(buf, 4096);

			// Wait for client to send data
			int bytesReceived = recv(clientSocket, buf, 4096, 0);
			if (bytesReceived == SOCKET_ERROR)
			{
				std::cerr << "Error in recv() function. Quitting..." << '\n';
				break;
			}
			if (bytesReceived == 0)
			{
				std::cerr << "Client disconnected. Quitting..." << '\n';
				break;
			}
			if (bytesReceived)
			//buf is the message
			std::cout << "CLIENT> " << std::string(buf, 0, bytesReceived) << endl;

			// Echo message back to client
			//send(clientSocket, buf, bytesReceived + 1, 0);
			string s;
			stringstream ss;
			ss << buf;

			ss >> s;
			SERVER::RECENTMESSAGE = s;
			if (s.substr(0, 5) == "START") {
				cout << "STARTING..." << endl;
				SERVER::RECENTMESSAGE = "";
				CLIENT::IS_STARTED = true;
			};
		}

#else
		while (true)
		{
			// Clear the buffer
			memset(buf, 0, 4096);
			// Wait for a message
			int bytesRecv = recv(clientSocket, buf, 4096, 0);
			if (bytesRecv == -1)
			{
				std::cerr << "There was a connection issue.\n";
				break;
			}

			if (bytesRecv == 0)
			{
				std::cout << "The client disconnected.\n";
				break;
			}

			// Display message
			std::cout << "Received: " << std::string(buf, 0, bytesRecv) << '\n';

			// Resend message
			send(clientSocket, buf, bytesRecv + 1, 0);
		}

#endif

		// Close the socket

#ifdef _WIN32
		closesocket(clientSocket);

		// Cleanup winsock
		WSACleanup();

#else
		close(clientSocket);

#endif
		return 0;

	};

	int SERVER::SEND(string data) {
		send(clientSocket, data.c_str(), data.length() + 1, 0);
		return 0;
	};

	int CLIENT::START(string ip) {
		// Create a hint structure for the server we are connecting with
		unsigned int port = 54000;
		std::string ipAddress = ip; //Host

		// Initialize WinSock
#ifdef _WIN32
		WSAData data;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &data);
		if (wsResult != 0)
		{
			std::cerr << "Can't start winsock, error #" << wsResult << '\n';
			return 1;
		}

#endif

		// Create a socket
#ifdef _WIN32
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			std::cerr << "Can't create socket, error #" << WSAGetLastError() << '\n';
			return 1;
		}

#else
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			return 1;
		}

#endif

		// Full in a hint structure
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

		// Connect to the server on the socket
		int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
		CLIENT::CONNECTED_TO_SERVER = true;

#ifdef _WIN32
		if (connectRes == SOCKET_ERROR)
		{
			std::cerr << "Cant't connect to server, err #" << WSAGetLastError() << '\n';
			WSACleanup();
			return 1;
		}

#else
		if (connectRes == -1)
		{
			return 1;
		}

#endif

		// Do-while loop to send and recieve data
		char buf[4096] = "";
		std::string userInput = " ";
#ifdef _WIN32
		while (true)
		{
			// Wait for response
			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);
			if (bytesReceived > 0)
			{
				// Echo response to console
				std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << '\n';
			}
			string s;
			stringstream ss;
			ss << buf;

			ss >> s;
			CLIENT::RECENTMESSAGE = s;
			cout << s << endl;
			if (s.substr(0, 5) == "START") {
				cout << "STARTING..." << endl;
				SERVER::IS_STARTED = true;
			}

			else if (s.substr(0, 5) == "NAME:") {
				if (namerecived == false) {
					namerecived = true;
					cout << "NAME RECIVED " << s << endl;
					CLIENT::NAME = s;
				};
			}
		}

#else
		do
		{
			// Enter lines of text
			std::cout << "> ";
			getline(std::cin, userInput);

			// Send to server
			int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			// Checking if that failed!
			if (sendRes == -1)
			{
				std::cout << "Could not send to server!\r\n";
				continue;
			}

			// Wait for response
			memset(buf, 0, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);
			if (bytesReceived == -1)
			{
				std::cout << "There was an error getting response from server.\r\n";
			}
			else
			{
				// Display response
				std::cout << "SERVER> " << std::string(buf, bytesReceived) << "\r\n";
			}
		} while (true);

#endif

		// Close the socket
#ifdef _WIN32
		closesocket(sock);
		WSACleanup();

#else
		close(sock);

#endif

		return 0;
	};

	int CLIENT::SEND(string data) {
		send(sock, data.c_str(), data.length() + 1, 0);
		return 0;
	}
}