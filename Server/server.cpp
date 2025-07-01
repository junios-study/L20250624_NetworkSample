#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX

#include <iostream>
#include <WinSock2.h>

#include "flatbuffers/flatbuffers.h"
#include "UserEvents_generated.h"
#include "Common.h"


#pragma comment(lib, "ws2_32")


int ProcessPacket(SOCKET ClientSocket, const char* RecvBuffer);
void CreateS2C_Login(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, bool IsSuccess, std::string Message);


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
		char RecvBuffer[4000] = { 0, };
		RecvPacket(ClientSocket, RecvBuffer);
		
		int Result = ProcessPacket(ClientSocket, RecvBuffer);
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}

int ProcessPacket(SOCKET ClientSocket, const char* RecvBuffer)
{
	//root_type
	auto RecvEventData = UserEvents::GetEventData(RecvBuffer);
	std::cout << RecvEventData->timestamp() << std::endl; //Å¸ÀÓ½ºÅÆÇÁ

	flatbuffers::FlatBufferBuilder Builder;

	switch (RecvEventData->data_type())
	{
		case UserEvents::EventType_C2S_Login:
		{
			auto LoginData = RecvEventData->data_as_C2S_Login();
			if (LoginData->userid() && LoginData->password())
			{
				std::cout << "Login Request success: " << LoginData->userid()->c_str() << ", " << LoginData->password()->c_str() << std::endl;
				CreateS2C_Login(Builder, ClientSocket, true, "Login Success");
			}
			else
			{
				CreateS2C_Login(Builder, ClientSocket, false, "empty id, password");
			}
			SendPacket(ClientSocket, Builder);
		}
		break;
		case UserEvents::EventType_C2S_Logout:
		{
			auto LoginData = RecvEventData->data_as_S2C_Logout();
		}
		break;
	}

	return 0;
}



void CreateS2C_Login(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, bool IsSuccess, std::string Message)
{
	auto LoginEvent = UserEvents::CreateS2C_Login(Builder, (uint32_t)ClientSocket, IsSuccess, Builder.CreateString(Message));
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_S2C_Login, LoginEvent.Union());
	Builder.Finish(EventData);
}