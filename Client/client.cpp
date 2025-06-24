#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")



int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(30303);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	srand(time(NULL));
	while (true)
	{
		Numbers Data;
		Data.FirstNumber = rand() % 9999 + 1;
		Data.SecondNumbers = rand() % 9999 + 1;

		std::cout << "First : " << Data.FirstNumber << std::endl;
		std::cout << "Second : " << Data.SecondNumbers << std::endl;


		Data.FirstNumber = htons(Data.FirstNumber);
		Data.SecondNumbers = htons(Data.SecondNumbers);

		unsigned short PacketSize = sizeof(Data);
		PacketSize = htons(PacketSize);
		send(ServerSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

		unsigned short EventName = (unsigned short)(rand() % 5 + 1);
		EventName = htons(EventName);
		send(ServerSocket, (char*)&EventName, sizeof(unsigned short), 0);

		send(ServerSocket, (char*)&Data, sizeof(Data), 0);



		int RecvByte = recv(ServerSocket, (char*)&PacketSize, 2, MSG_WAITALL);
		PacketSize = ntohs(PacketSize);

		RecvByte = recv(ServerSocket, (char*)&EventName, 2, MSG_WAITALL);
		EventName = ntohs(EventName);

		EEventCode PakcetCode = (EEventCode)EventName;

		switch (PakcetCode)
		{
			case EEventCode::S2C_Result:
			{
				ResultNumber ResultData;
				RecvByte = recv(ServerSocket, (char*)&ResultData, PacketSize, MSG_WAITALL);

				ResultData.Number = ntohs(ResultData.Number);

				std::cout << "Result = " << ResultData.Number << std::endl;
			}
		}
	}






	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}