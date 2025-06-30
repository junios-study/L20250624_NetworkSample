#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "Packet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "flatbuffers/flatbuffers.h"
#include "Calculate_generated.h"
#include "Result_generated.h"


#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(30303);

	::connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	char Operators[5] = { '+', '-', '*', '/', '%' };
	srand((unsigned int)time(NULL));
	while (true)
	{
		int Number1 = rand() % 9998 + 1;
		int Number2 = rand() % 9998 + 1;
		uint8_t Operator = Operators[rand() % 5];

		flatbuffers::FlatBufferBuilder Builder(1024);
		auto Data = Calculate::CreateData(Builder, Number1, Number2, Operator);
		Builder.Finish(Data);

		std::cout << Number1 << " " << Operator << " " << Number2;

		int PacketSize = (int)Builder.GetSize();
		PacketSize = ::htonl(PacketSize);
		//header, 길이
		int SentBytes = ::send(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), 0);
		//자료 
		SentBytes = ::send(ServerSocket, (char*)Builder.GetBufferPointer(), Builder.GetSize(), 0);

		//받기
		char RecvBuffer[1024] = { 0, };
		int RecvBytes = recv(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
		PacketSize = ntohl(PacketSize);
		RecvBytes = recv(ServerSocket, RecvBuffer, PacketSize, MSG_WAITALL);

		auto d = Calculate::GetResult(RecvBuffer);


		std::cout << " = " << d->result_number() << std::endl;
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}