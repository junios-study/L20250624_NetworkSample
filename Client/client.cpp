#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#include "flatbuffers/flatbuffers.h"
#include "UserEvents_generated.h"

#include "Common.h"


#pragma comment(lib, "ws2_32")

void CreateC2S_Login(flatbuffers::FlatBufferBuilder& Builder);
void ProcessPacket(SOCKET ServerSocket, const char* RecvBuffer);




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

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	while (true)
	{
		char RecvBuffer[10240] = { 0 };
		flatbuffers::FlatBufferBuilder Builder;

		CreateC2S_Login(Builder);

		SendPacket(ServerSocket, Builder);
		
		RecvPacket(ServerSocket, RecvBuffer);

		ProcessPacket(ServerSocket, RecvBuffer);
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}

void CreateC2S_Login(flatbuffers::FlatBufferBuilder& Builder)
{
	auto LoginEvent = UserEvents::CreateC2S_Login(Builder, Builder.CreateString("username"), Builder.CreateString("password"));
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_C2S_Login, LoginEvent.Union());
	Builder.Finish(EventData);
}


void ProcessPacket(SOCKET ServerSocket, const char* RecvBuffer)
{
	//root_type
	auto RecvEventData = UserEvents::GetEventData(RecvBuffer);
	std::cout << RecvEventData->timestamp() << std::endl; //Ÿ�ӽ�����

	switch (RecvEventData->data_type())
	{
	case UserEvents::EventType_S2C_Login:
	{
		auto LoginData = RecvEventData->data_as_S2C_Login();
	}
	break;
	}
}

