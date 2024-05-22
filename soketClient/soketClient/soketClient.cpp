#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	setlocale(LC_ALL, "RUS");

	WSADATA wasData;// структура для хранения информации
	SOCKET ConnectSocket = INVALID_SOCKET; // сокет для подключения к серверу


	ADDRINFO hints; // информация о сокете
	ADDRINFO* addrResult; // адрес хоста
	const char* senBuffer = "Hello from client";
	const char* senBuffer2 = "Ура я могу да"; // сообщение для сервера
	char recvBuffer[512];// массив символов  для приема сообщения

	int result = WSAStartup(MAKEWORD(2, 2), &wasData);// определение версии и структуры при помощи библиотеки сокетов 
	if (result != 0) {
		cout << " WSAStartup FAILED result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));// зануление памяти (что ,сколько)
	hints.ai_family = AF_INET;// присвоение 4Байтного эзернета
	hints.ai_socktype = SOCK_STREAM;// потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// протокол TCP

	// result - адрес, порт, семейство структур и адрес сокета
	result = getaddrinfo("localhost", "666", &hints, &addrResult);
	if (result != 0) {
		cout << " getaddrinfo FAILED result" << endl;
		WSACleanup(); // очистка WSAStartup
		return 1;
	}

	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);// создание сокета (адрес, тип сокета, протокол)
	if (ConnectSocket == INVALID_SOCKET) {
		cout << " ConnectSocket FAILED result" << endl;
		WSACleanup();
		FreeAddrInfo(addrResult);// освобождение памяти у адреса
		return 1;
	}

	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);	// установка соединения (дескриптор сокета для подключения,адрес, размер адреса хоста)
	if (result == SOCKET_ERROR) {
		cout << " Connection FAILED result" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = send(ConnectSocket, senBuffer, (int)strlen(senBuffer), 0);// отправка сообщения (дескриптор сокета подключения, сообщение, количество байтов и флаг)
	if (result == SOCKET_ERROR) {
		cout << " send FAILED result" << endl;
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << " bytes" << endl;

	// отправка 2 сообщения
	result = send(ConnectSocket, senBuffer2, (int)strlen(senBuffer2), 0);
	if (result == SOCKET_ERROR) {
		cout << " send FAILED result" << endl;
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << " bytes" << endl;

	result = shutdown(ConnectSocket, SD_SEND);// отключение от сервера (дескриптор сокета подключения, флаг на запрет отправки данных)
	if (result == SOCKET_ERROR) {
		cout << " shutdown FAILED result" << endl;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	// вывод сообщения
	do {
		ZeroMemory(recvBuffer, 512);// зануление буфера
		result = recv(ConnectSocket, recvBuffer, 512, 0);// получение данных с сокета ( дескриптор сокета подключения, буфер, размер буфера, флаг)
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;// вывод отправленных байтов
			cout << "Received data " << recvBuffer << endl;// вывод сообщения

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;

	} while (result > 0);
	freeaddrinfo(addrResult);/// освобождение памяти для addrResult
	WSACleanup();//доинициализируем библиотеку
	return 0;
}
