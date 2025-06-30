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
	const char* JSONString = "{\"Key\" :		\"Value1\",		\n\n  \"Key1\" : \"Value1\",  \"Number1\" : 10, \"Number2\" : 20 }";

	
	//°´Ã¼, deserialize 
	Document d;

	d.Parse(JSONString);
	Document::AllocatorType& allocator = d.GetAllocator();
	d.AddMember("Number3", d["Number1"].GetInt() + d["Number2"].GetInt(), d.GetAllocator());

	std::cout << d["Number3"].GetInt() << endl;


	//serialize
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	std::cout << buffer.GetString() << endl;


	//WSAData wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);

	//SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//SOCKADDR_IN ServerSockAddr;
	//memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	//ServerSockAddr.sin_family = PF_INET;
	//ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//ServerSockAddr.sin_port = htons(30303);

	//connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	//srand(time(NULL));
	//while (true)
	//{
	//}

	//closesocket(ServerSocket);

	//WSACleanup();

	return 0;
}