#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX

#include <iostream>
#include <WinSock2.h>

#include "flatbuffers/flatbuffers.h"
#include "UserEvents_generated.h"
#include "Common.h"


#pragma comment(lib, "ws2_32")


int ProcessPacket(SOCKET ClientSocket, const char* RecvBuffer);
void CreateS2C_Login(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, bool IsSuccess, std::string Message, int X = 0, int Y = 0, const UserEvents::Color* color = nullptr);

void CreateS2C_Logout(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket);
void CreateS2C_PlayerMoveData(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, int X, int Y);

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
		int RecvBytes = RecvPacket(ClientSocket, RecvBuffer);
		if (RecvBytes <= 0)
		{
			std::cout << "Client disconnected." << std::endl;
			break; // 클라이언트가 연결을 끊었을 때 루프 종료
		}

		int Result = ProcessPacket(ClientSocket, RecvBuffer);
		if (Result < 0)
		{
			std::cout << "Client requested to disconnect." << std::endl;
			break; // 클라이언트가 로그아웃을 요청했을 때 루프 종료
		}
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}

int ProcessPacket(SOCKET ClientSocket, const char* RecvBuffer)
{
	//root_type
	auto RecvEventData = UserEvents::GetEventData(RecvBuffer);
	std::cout << RecvEventData->timestamp() << std::endl; //타임스탬프

	flatbuffers::FlatBufferBuilder SendBuilder;

	switch (RecvEventData->data_type())
	{
		case UserEvents::EventType_C2S_Login:
		{
			auto LoginData = RecvEventData->data_as_C2S_Login();
			if (LoginData->userid() && LoginData->password())
			{
				std::cout << "Login Request success: " << LoginData->userid()->c_str() << ", " << LoginData->password()->c_str() << std::endl;
				CreateS2C_Login(SendBuilder, ClientSocket, true, "Login Success", 10, 10);
			}
			else
			{
				CreateS2C_Login(SendBuilder, ClientSocket, false, "empty id, password");
			}
			SendPacket(ClientSocket, SendBuilder);
		}
		break;
		case UserEvents::EventType_C2S_PlayerMoveData:
		{
			std::cout << "EventType_C2S_PlayerMoveData" << std::endl;
			auto PlayerMoveData = RecvEventData->data_as_C2S_PlayerMoveData();
			int PlayerX = PlayerMoveData->position_x();
			int PlayerY = PlayerMoveData->position_y();
			if (PlayerMoveData->key_code() != 0)
			{
				switch (toupper(PlayerMoveData->key_code()))
				{
					case 'W':
					{
						PlayerY--;
						break;
					}
					case 'S':
					{
						PlayerY++;
						break;
					}
					case 'A':
					{
						PlayerX--;
						break;
					}
					case 'D':
					{
						PlayerX++;
						break;
					}
					case 27:
					{
						CreateS2C_Logout(SendBuilder, ClientSocket);
						SendPacket(ClientSocket, SendBuilder);
						break;
					}
				}

				PlayerX = std::clamp(PlayerX, 0, 100);
				PlayerY = std::clamp(PlayerY, 0, 100);
			}

			GotoXY(PlayerX, PlayerY);
			std::cout << "P" << std::endl;

			CreateS2C_PlayerMoveData(SendBuilder, ClientSocket, PlayerX, PlayerY);
			SendPacket(ClientSocket, SendBuilder);
		}
		break;
		case UserEvents::EventType_C2S_Logout:
		{
			auto LoginData = RecvEventData->data_as_S2C_Logout();
			return -1;
		}
		break;
	}

	return 0;
}



void CreateS2C_Login(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, bool IsSuccess, std::string Message, int X, int Y, const UserEvents::Color* color)
{
	auto LoginEvent = UserEvents::CreateS2C_Login(Builder, (uint32_t)ClientSocket, IsSuccess, Builder.CreateString(Message), X, Y, color);
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_S2C_Login, LoginEvent.Union());
	Builder.Finish(EventData);
}

void CreateS2C_Logout(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket)
{
	auto LogoutEvent = UserEvents::CreateS2C_Logout(Builder, (uint32_t)ClientSocket);
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_S2C_Logout, LogoutEvent.Union());
	Builder.Finish(EventData);
}

void CreateS2C_PlayerMoveData(flatbuffers::FlatBufferBuilder& Builder, SOCKET ClientSocket, int X, int Y)
{
	auto SendPlayerMoveData = UserEvents::CreateS2C_PlayerMoveData(Builder, (uint32_t)ClientSocket, X, Y);
	auto EventData = UserEvents::CreateEventData(Builder, GetTimeStamp(), UserEvents::EventType_S2C_PlayerMoveData, SendPlayerMoveData.Union());
	Builder.Finish(EventData);
}