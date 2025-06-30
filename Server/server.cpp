#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX

#include <iostream>
#include <WinSock2.h>

#include "flatbuffers/flatbuffers.h"
#include "Calculate_generated.h"
#include "Result_generated.h"

#pragma comment(lib, "ws2_32")

int ProcessPacket(const char* buffer);


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
		int PacketSize = 0;
		char RecvBuffer[4000] = { 0, };
		int RecvBytes = recv(ClientSocket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
		PacketSize = ntohl(PacketSize);
		RecvBytes = recv(ClientSocket, RecvBuffer, PacketSize, MSG_WAITALL);

		int Result = ProcessPacket(RecvBuffer);

		flatbuffers::FlatBufferBuilder Builder(1024);
		auto CalculateResult = Calculate::CreateResult(Builder, Result);
		Builder.Finish(CalculateResult);

		PacketSize = (int)Builder.GetSize();
		PacketSize = htonl(PacketSize);
		int SentBytes = send(ClientSocket, (char*)&PacketSize, sizeof(PacketSize), 0);
		SentBytes = send(ClientSocket, (char*)Builder.GetBufferPointer(), Builder.GetSize(), 0);
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}

int ProcessPacket(const char* buffer)
{
	auto d = Calculate::GetData(buffer);
	
	switch (d->operator_())
	{
	case '+':
		return d->number1() + d->number2();
	case '-':
		return d->number1() - d->number2();
	case '/':
		return d->number1() /+ d->number2();
	case '*':
		return d->number1() * d->number2();
	case '%':
		return d->number1() % d->number2();
	}

	return 0;
}