#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<Winsock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<stdint.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

void main()
{

	setlocale(LC_ALL, "Russian");
	cout << "WinSock Client" << endl;

	//1)  Инициализация WinSock
	WSADATA wsaData;
	INT iResult = (WSAStartup(MAKEWORD(2, 2), &wsaData));
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}

	//2) Создаем сокет клиента ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // INET -  TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//3) Определяем IP-адрес сервера
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		cout << "getaddressinfo() failed with code " << iResult << endl;
		WSACleanup();
		return;
	}
	cout << "hints:" << endl;
	cout << "ai_addr:" << hints.ai_addr->sa_data << endl;

	//?) Освобождаем ресурсы WinSock
	WSACleanup();
}