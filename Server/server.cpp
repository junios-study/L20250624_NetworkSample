#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include "Packet.h"

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

		//char Buffer[1024] = { 0, };
		//int AssignTotalByte = 2;
		//int RecvTotaltByte = 0;
		//do
		//{
		//	int RecvByte = recv(ClientSocket, &Buffer[RecvTotaltByte], AssignTotalByte - RecvTotaltByte, 0);
		//	int RecvByte = recv(ClientSocket, Buffer, 2, 0);
		//	RecvTotaltByte += RecvByte;
		//} while (RecvTotaltByte < AssignTotalByte);

		//size   code(65535)
		//[][]   [][]   [][][][][][]
		//블락킹 모드 성능 좋지 않아요.
		unsigned short PacketSize; //[][]
		int RecvByte = recv(ClientSocket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
		PacketSize = ntohs(PacketSize);

		unsigned short EventName;
		RecvByte = recv(ClientSocket, (char*)&EventName, sizeof(PacketSize), MSG_WAITALL);
		EventName = ntohs(EventName);

		switch ((EEventCode)EventName)
		{
			case EEventCode::C2S_Plus:
			{
				Numbers Data;
				RecvByte = recv(ClientSocket, (char*)&Data, PacketSize, MSG_WAITALL);

				Data.FirstNumber = ntohs(Data.FirstNumber);
				Data.SecondNumbers = ntohs(Data.SecondNumbers);

				int Result = Data.FirstNumber + Data.SecondNumbers;

				PacketSize = sizeof(Result);
				PacketSize = htons(PacketSize);
				send(ClientSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

				unsigned short EventName = (unsigned short)EEventCode::S2C_Result;
				EventName = htons(EventName);
				send(ClientSocket, (char*)&EventName, sizeof(unsigned short), 0);

				ResultNumber R;
				R.Number = Result;
				R.Number = htons(R.Number);
				send(ClientSocket, (char*)&R, sizeof(R), 0);
				break;
			}
			case EEventCode::C2S_Minus:
			{
				Numbers Data;
				RecvByte = recv(ClientSocket, (char*)&Data, PacketSize, MSG_WAITALL);

				Data.FirstNumber = ntohs(Data.FirstNumber);
				Data.SecondNumbers = ntohs(Data.SecondNumbers);

				int Result = Data.FirstNumber - Data.SecondNumbers;

				PacketSize = sizeof(Result);
				PacketSize = htons(PacketSize);
				send(ClientSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

				unsigned short EventName = (unsigned short)EEventCode::S2C_Result;
				EventName = htons(EventName);
				send(ClientSocket, (char*)&EventName, sizeof(unsigned short), 0);

				ResultNumber R;
				R.Number = Result;
				R.Number = htons(R.Number);
				send(ClientSocket, (char*)&R, sizeof(R), 0);
				break;
			}
			case EEventCode::C2S_Multiply:
			{
				Numbers Data;
				RecvByte = recv(ClientSocket, (char*)&Data, PacketSize, MSG_WAITALL);

				Data.FirstNumber = ntohs(Data.FirstNumber);
				Data.SecondNumbers = ntohs(Data.SecondNumbers);

				int Result = Data.FirstNumber * Data.SecondNumbers;

				PacketSize = sizeof(Result);
				PacketSize = htons(PacketSize);
				send(ClientSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

				unsigned short EventName = (unsigned short)EEventCode::S2C_Result;
				EventName = htons(EventName);
				send(ClientSocket, (char*)&EventName, sizeof(unsigned short), 0);

				ResultNumber R;
				R.Number = Result;
				R.Number = htons(R.Number);
				send(ClientSocket, (char*)&R, sizeof(R), 0);
				break;
			}
			case EEventCode::C2S_Divide:
			{
				Numbers Data;
				RecvByte = recv(ClientSocket, (char*)&Data, PacketSize, MSG_WAITALL);

				Data.FirstNumber = ntohs(Data.FirstNumber);
				Data.SecondNumbers = ntohs(Data.SecondNumbers);

				int Result = Data.FirstNumber / Data.SecondNumbers;

				PacketSize = sizeof(Result);
				PacketSize = htons(PacketSize);
				send(ClientSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

				unsigned short EventName = (unsigned short)EEventCode::S2C_Result;
				EventName = htons(EventName);
				send(ClientSocket, (char*)&EventName, sizeof(unsigned short), 0);

				ResultNumber R;
				R.Number = Result;
				R.Number = htons(R.Number);
				send(ClientSocket, (char*)&R, sizeof(R), 0);
				break;
			}
			case EEventCode::C2S_Remain:
			{
				Numbers Data;
				RecvByte = recv(ClientSocket, (char*)&Data, PacketSize, MSG_WAITALL);

				Data.FirstNumber = ntohs(Data.FirstNumber);
				Data.SecondNumbers = ntohs(Data.SecondNumbers);

				int Result = Data.FirstNumber % Data.SecondNumbers;

				PacketSize = sizeof(Result);
				PacketSize = htons(PacketSize);
				send(ClientSocket, (char*)&PacketSize, sizeof(unsigned short), 0);

				unsigned short EventName = (unsigned short)EEventCode::S2C_Result;
				EventName = htons(EventName);
				send(ClientSocket, (char*)&EventName, sizeof(unsigned short), 0);

				ResultNumber R;
				R.Number = Result;
				R.Number = htons(R.Number);
				send(ClientSocket, (char*)&R, sizeof(R), 0);
				break;
			}
		}
	}






	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}