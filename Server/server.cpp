#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include "Packet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#pragma comment(lib, "ws2_32")

using namespace std;
using namespace rapidjson;

int ProcessPacket(Document& d);

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

		cout << RecvBuffer << endl;

		Document d;
		d.Parse(RecvBuffer);

		int Result = ProcessPacket(d);

		d.RemoveAllMembers();
		d.SetObject();
		d.AddMember("Result", Result, d.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);

		cout << buffer.GetString() << endl;

		PacketSize = (int)buffer.GetSize();
		PacketSize = htonl(PacketSize);
		int SentBytes = send(ClientSocket, (char*)&PacketSize, sizeof(PacketSize), 0);
		SentBytes = send(ClientSocket, buffer.GetString(), (int)buffer.GetSize(), 0);
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}

int ProcessPacket(Document& d)
{
	switch (d["Operator"].GetInt())
	{
		case '+':
			return d["Number1"].GetInt() + d["Number2"].GetInt();
		case '-':
			return d["Number1"].GetInt() - d["Number2"].GetInt();
		case '/':
			return d["Number1"].GetInt() / d["Number2"].GetInt();
		case '*':
			return d["Number1"].GetInt() * d["Number2"].GetInt();
		case '%':
			return d["Number1"].GetInt() % d["Number2"].GetInt();
	}

	return 0;
}