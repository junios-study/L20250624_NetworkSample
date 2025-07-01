#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>

#include "flatbuffers/flatbuffers.h"
#include "UserEvents_generated.h"

#include "Common.h"
	


#pragma comment(lib, "ws2_32")



int ProcessPacket(SOCKET ServerSocket, const char* RecvBuffer);

void CreateC2S_Login(flatbuffers::FlatBufferBuilder& Builder);
void CreateC2S_PlayerMoveData(flatbuffers::FlatBufferBuilder& Builder, int playerId, int positionX, int positionY, int keyCode);





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

	char RecvBuffer[10240] = { 0 };
	flatbuffers::FlatBufferBuilder Builder;

	CreateC2S_Login(Builder);

	SendPacket(ServerSocket, Builder);

	while (true)
	{
		int RecvBytes = RecvPacket(ServerSocket, RecvBuffer);
		if (RecvBytes <= 0)
		{
			std::cout << "서버와의 연결이 끊어졌습니다." << std::endl;
			break;
		}

		int Result = ProcessPacket(ServerSocket, RecvBuffer);
		if (Result < 0)
		{
			std::cout << "서버와의 연결이 종료되었습니다." << std::endl;
			break;
		}
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


int ProcessPacket(SOCKET ServerSocket, const char* RecvBuffer)
{
	flatbuffers::FlatBufferBuilder SendBuilder;
	//root_type
	auto RecvEventData = UserEvents::GetEventData(RecvBuffer);
	std::cout << RecvEventData->timestamp() << std::endl; //타임스탬프

	switch (RecvEventData->data_type())
	{
		case UserEvents::EventType_S2C_Login:
		{
			auto LoginData = RecvEventData->data_as_S2C_Login();
			if (LoginData)
			{
				std::cout << "로그인 성공: " << LoginData->success() << std::endl;
				std::cout << "유저 ID: " << LoginData->player_id() << std::endl;

				CreateC2S_PlayerMoveData(SendBuilder, LoginData->player_id(), 10, 10, 0);
				SendPacket(ServerSocket, SendBuilder);
			}
			else
			{
				std::cout << "로그인 데이터가 유효하지 않습니다." << std::endl;
			}
		}
		break;
		case UserEvents::EventType_S2C_PlayerMoveData:
		{
			std::cout << "EventType_S2C_PlayerMoveData" << std::endl;
			auto PlyerMoveData = RecvEventData->data_as_S2C_PlayerMoveData();
			if (PlyerMoveData)
			{
				GotoXY(PlyerMoveData->position_x(), PlyerMoveData->position_y());
				std::cout << "P" << std::endl;

				int keyCode = _getch();
				CreateC2S_PlayerMoveData(SendBuilder, PlyerMoveData->player_id(), PlyerMoveData->position_x(), PlyerMoveData->position_y(), keyCode);
				SendPacket(ServerSocket, SendBuilder);
			}
			else
			{
				std::cout << "플레이어 이동 데이터가 유효하지 않습니다." << std::endl;
			}
		}
		break;
		case UserEvents::EventType_S2C_Logout:
		{
			return -1;
		}
		break;
	}

	return 0;
}


void CreateC2S_PlayerMoveData(flatbuffers::FlatBufferBuilder& Builder, int playerId, int positionX, int positionY, int keyCode)
{
	auto PlayerMoveData = UserEvents::CreateC2S_PlayerMoveData(Builder, playerId, positionX, positionY, keyCode);
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_C2S_PlayerMoveData, PlayerMoveData.Union());
	Builder.Finish(EventData);
}