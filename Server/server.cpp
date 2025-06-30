#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include "Packet.h"
#include "rapidjson/document.h"

#pragma comment(lib, "ws2_32")



int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = PF_INET;
	ListenSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ListenSockAddr.sin_port = htons(30303);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	SOCKADDR_IN ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int ClientSockAddrLength = sizeof(ClientSockAddr);
	SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);

	while (true)
	{
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}