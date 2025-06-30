#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define NOMINMAX


#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "Packet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#pragma comment(lib, "ws2_32")

using namespace std;
using namespace rapidjson;

int main()
{
	//serialize(file, socket)
	//const char* JSONString = "{\"Key\" :		\"Value1\",		\n\n  \"Key1\" : \"Value1\",  \"Number1\" : 10, \"Number2\" : 20, \"Image\"  : \"sdfsdfsdsdfdsfdsfsd\"  }";

	////base64 encoding ->  base64 decoding

	////객체, deserialize 
	//Document d;

	//d.Parse(JSONString);
	//Document::AllocatorType& allocator = d.GetAllocator();
	//d.AddMember("Number3", d["Number1"].GetInt() + d["Number2"].GetInt(), d.GetAllocator());

	//std::cout << d["Number3"].GetInt() << endl;


	////serialize
	//StringBuffer buffer;
	//Writer<StringBuffer> writer(buffer);
	//d.Accept(writer);

	//std::cout << buffer.GetString() << endl;

	//recv
	//숫자 2개, 명령어 연산자.
	//문자열 길이, 패킷(길이), 4바이트, 42억byte
	//{
	//	"Number1" : 10,
	//	"Number2" : 20,
	//	"Operator" : "+"
	//}

	//C#, java, nodejs, python -> 한줄로 파싱 Parse, ToString()
	//{
	//	"Result" : 30
	//}


	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = PF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSockAddr.sin_port = htons(30303);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	char Operators[5] = { '+', '-', '*', '/', '%' };
	srand(time(NULL));
	while (true)
	{
		int Number1 = rand() % 9998 + 1;
		int Number2 = rand() % 9998 + 1;

		Document d;
		d.SetObject();
		d.AddMember("Number1", Number1, d.GetAllocator());
		d.AddMember("Number2", Number2, d.GetAllocator());
		d.AddMember("Operator", Operators[rand() % 5], d.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);

		cout << buffer.GetString() << endl;

		int PacketSize = buffer.GetSize();
		PacketSize = htonl(PacketSize);
		int SentBytes = send(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), 0);
		SentBytes = send(ServerSocket, buffer.GetString(), buffer.GetSize(), 0);

		char RecvBuffer[65535] = { 0, };
		int RecvBytes = recv(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
		PacketSize = ntohl(PacketSize);
		RecvBytes = recv(ServerSocket, RecvBuffer, PacketSize, MSG_WAITALL);

		cout << RecvBuffer << endl;

		Document d2;
		d2.Parse(RecvBuffer);
		cout << d2["Result"].GetInt() << endl;
	}

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}