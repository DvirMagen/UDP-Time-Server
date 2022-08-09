#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>

#define TIME_PORT	27015

void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	time_t timer;
	time_t current_timer;
	bool measurement_flag = false;
	struct tm* temp_timer;

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		// Answer client's request by the current time.

		// Get the current time.
		
		time(&timer);
		temp_timer = localtime(&timer);
		int rec_buff_integer = stoi(recvBuff);
		// Parse the current time to printable string.
		if (strcmp(recvBuff, "1") == 0)
		{
			strcpy(sendBuff, ctime(&timer));
			sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
		}
		else if (strcmp(recvBuff, "2") == 0)
		{
			strftime(sendBuff, 80, "%X", temp_timer);
		}
		else if (strcmp(recvBuff, "3") == 0)
		{
			timer = time(&timer);
			sprintf(sendBuff, "%d", (int)timer);
		}
		else if (strcmp(recvBuff, "4") == 0)
		{
			strcpy(sendBuff, "Return From 4 Function");
		}
		else if (strcmp(recvBuff, "5") == 0)
		{
			strcpy(sendBuff, "Return From 5 Function");
		}
		else if (strcmp(recvBuff, "6") == 0)
		{
			strftime(sendBuff, 80, "%H:%M", temp_timer);
		}
		else if (strcmp(recvBuff, "7") == 0)
		{
			strftime(sendBuff, 80, "%Y", temp_timer);
		}
		else if (strcmp(recvBuff, "8") == 0)
		{
			strftime(sendBuff, 80, "Today is %A, The month is %B", temp_timer);

		}
		else if (strcmp(recvBuff, "9") == 0)
		{
			int sec_in_month = 0;
			int temp = 0;
			int day_in_month = temp_timer->tm_mday;
			int sec_in_day = temp_timer->tm_sec;
			int min_in_day = temp_timer->tm_min;
			int hours_in_day = temp_timer->tm_hour;
			--day_in_month;
			temp = day_in_month * 24 * 60 * 60;
			sec_in_month = temp + sec_in_day + (min_in_day) * 60 + (hours_in_day) * 3600;
			sprintf(sendBuff, "%d", sec_in_month);

		}
		else if (strcmp(recvBuff, "10") == 0)
		{
			strftime(sendBuff, 80, "%U", temp_timer);

		}
		else if (strcmp(recvBuff, "11") == 0)
		{
			int isDayLight = temp_timer->tm_isdst;
			sprintf(sendBuff, "%d", isDayLight);
		}
		else if (recvBuff[0] == '1' && recvBuff[1] == '2' && recvBuff[2] == ' ')
		{
			time_t now = time(0);
			tm* now_tm = gmtime(&now);
			int h_now_tm = now_tm->tm_hour;
			int min_now_tm = now_tm->tm_min;
			int sec_now_tm = now_tm->tm_sec;
			if (strcmp(recvBuff, "12 1") == 0) /*Tokyo*/
			{
				h_now_tm += 9;
				if (h_now_tm > 24)
				{
					h_now_tm -= 24;
				}
				sprintf(sendBuff, "%d:%d:%d", h_now_tm, min_now_tm, sec_now_tm);
			}
			else if (strcmp(recvBuff, "12 2") == 0) /*Melbourne */
			{
				h_now_tm += 10;
				if(h_now_tm > 24)
				{
					h_now_tm -= 24;
				}
				sprintf(sendBuff, "%d:%d:%d", h_now_tm, min_now_tm, sec_now_tm);
			}
			else if (strcmp(recvBuff, "12 3") == 0) /*San-Francisco*/
			{
				h_now_tm -= 7;
				if (h_now_tm < 0)
				{
					h_now_tm += 24;
				}
				sprintf(sendBuff, "%d:%d:%d", h_now_tm, min_now_tm, sec_now_tm);
			}
			else if (strcmp(recvBuff, "12 4") == 0) /*Porto*/
			{
				h_now_tm += 1;
				if (h_now_tm > 24)
				{
					h_now_tm -= 24;
				}
				sprintf(sendBuff, "%d:%d:%d", h_now_tm, min_now_tm, sec_now_tm);
			}
			else
			{
				strftime(sendBuff, 80, "%X +UTC", now_tm);
			}
		}
		else if (strcmp(recvBuff, "13") == 0)
		{
			
			if(!measurement_flag ||( (timer - current_timer) >= 180))
			{
				time(&current_timer);
				strcpy(sendBuff, "Time measurement begin...");
				measurement_flag = true;
			}
			else
			{
				current_timer = timer - current_timer;
				sprintf(sendBuff, "%d sec passed", (int)current_timer);
				time(&current_timer);
				
			}

		}

		
		// Sends the answer to the client, using the client address gathered
		// by recvfrom.
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);

		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}