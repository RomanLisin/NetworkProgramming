#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>
#include<iostream>
#include<FormatLastError.h>
using namespace std;


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 1500

void main()
{
	setlocale(LC_ALL, "");

	//1) Инициализация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		PrintLastError(WSAGetLastError());
		return;
	}

	//2) проверяем, не занят ли нужный нам порт
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with ";
		PrintLastError(WSAGetLastError());
		WSACleanup();
		return;
	}

	//3) создаем сокет, который будет слушать и ожидать подключения от клиента
	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "socket() failed with ";
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//4) BindSocket - связываем Сокет с целевым IP-адресом и портом:
	iResult = bind(listen_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//5) включаем прослушивание Сокета:
	iResult = listen(listen_socket, SOMAXCONN /*покажет нам сколько одновременно может тащить этот сокет*/);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//6) Принимаем запросы на соединение от клиентов:
	cout << "Wait for clients..." << endl;
	SOCKET client_socket = accept(listen_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		cout << "accept() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//7) Получение и отправка данных:
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};
	do
	{
		iResult = recv(client_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0); // возвращает количество байт
		if (iResult > 0)
		{
			cout << "Received Bytes: " << iResult << ", Message: " << recvbuffer << endl;
			if (send(client_socket, recvbuffer, strlen(recvbuffer), 0) == SOCKET_ERROR)
			{
				cout << "send() failed with ";
				PrintLastError(WSAGetLastError());
				closesocket(client_socket);
				break;
			}
		}
			else if (iResult == 0) cout << "Connection closing..." << endl;
			else
		{
			cout << "recv() failed with ";
			 PrintLastError(WSAGetLastError());
		}
	} while (iResult > 0);
	// ? Освобождение ресурсов WinSiock:
	closesocket(client_socket);
	closesocket(listen_socket);
	freeaddrinfo(result);
	WSACleanup();


}