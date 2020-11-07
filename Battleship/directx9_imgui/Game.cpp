#include <iostream>
#include <atomic>
#include <sstream>
using namespace std;

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
SOCKET clientSocket;
SOCKET sock;
#include "Game.h"
namespace std {
	bool SERVER::IS_STARTED = false;

	bool CLIENT::IS_STARTED = false;


	bool SERVER::CONNECTED_TO_CLIENT = false;

	bool CLIENT::CONNECTED_TO_SERVER = false;

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
			send(clientSocket, buf, bytesReceived + 1, 0);
			string s;
			stringstream ss;
			ss << buf;

			ss >> s;

			if (s.substr(0, 5) == "START") {
				cout << "STARTING..." << endl;
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
	}

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
		std::string userInput;

#ifdef _WIN32
		do
		{
			// Prompt the user for some text
			std::cout << "> ";
			std::cin >> userInput;
			if (userInput.size() > 0) // Make sure the user has typed something
			{
				// Send the text
				int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
				if (sendResult != SOCKET_ERROR)
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
					if (s.substr(0, 5) == "START") {
						cout << "STARTING..." << endl;
						SERVER::IS_STARTED = true;
					}
				}
			}

		} while (userInput.size() > 0);

		cout << "0 bytes" << endl;

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
	}
}