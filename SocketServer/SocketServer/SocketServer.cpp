#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	setlocale(LC_ALL, "RUS");

	WSADATA wasData;// структура для хранения информации
	SOCKET ClientSocket = INVALID_SOCKET; // сокет для подключения

	ADDRINFO hints; // информация о сокете
	ADDRINFO* addrResult; // адрес хоста
	const char* senBuffer = "Hello from server"; // сообщение от сервера клиенту 
	char recvBuffer[512]; // массив символов для приема сообщения
	char recvBuffer2[512]; // массив символов  для приема сообщения

	SOCKET ListenSocket = INVALID_SOCKET; // прослушивающий сокет

	int result = WSAStartup(MAKEWORD(2, 2), &wasData); // определение версии и структуры при помощи библиотеки сокетов 
	if (result != 0) {
		cout << " WSAStartup FAILED result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints)); // зануление памяти (что ,сколько)
	hints.ai_family = AF_INET; // присвоение 4Байтного эзернета
	hints.ai_socktype = SOCK_STREAM;// потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// протокол TCP
	hints.ai_flags = AI_PASSIVE; // Ждёт сообщения

	// result - адрес, порт, семейство структур и адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	if (result != 0) {
		cout << " getaddrinfo FAILED result" << endl;
		WSACleanup();
		return 1;
	}

	// создание сокета ждули (адрес, тип сокета, протокол)
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << " ListenSocket FAILED result" << endl;
		WSACleanup(); // очистка WSAStartup
		FreeAddrInfo(addrResult); // освобождение памяти у адреса
		return 1;
	}

	// установка соедниения (дескриптор сокета прослушивания, структура, длинна структуры)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << " binding connect" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}

	// перевод сокета в режим прослушивания (сокет, константа)
	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << " ListenSocket connect" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	// заполнение сокета прослушивания (дескриптор сокета прослушивания, NULL для адреса и длины адреса)
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << " accept FAILED result" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);// закрытие сокета прослушивания

	do {
		ZeroMemory(recvBuffer, 512); //зануление буфера
		result = recv(ClientSocket, recvBuffer, 512, 0);// result -  данные от сокета клиента (дескриптор сокета клиента, сообщение, размер буфера, флаг)
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl; //вывод принятых байтов
			cout << "Received data " << recvBuffer << endl;// сообщение

			result = send(ClientSocket, senBuffer, (int)strlen(senBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << " send FAILED result" << endl;
				WSACleanup();
				return 1;
			}

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;
		// приём 2 сообщения
		ZeroMemory(recvBuffer2, 512);
		result = recv(ClientSocket, recvBuffer2, 512, 0);
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer2 << endl;

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;
		closesocket(ClientSocket); // закрытие сокета клиента
		FreeAddrInfo(addrResult); // очистка адреса хоста
		WSACleanup();
		return 1;


	} while (result > 0);
	result = shutdown(ClientSocket, SD_SEND); // отключение (дескриптор сокета клиента, флаг)
	if (result == SOCKET_ERROR) {
		cout << " shutdown FAILED result" << endl;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);// закрытие сокета клиента
	freeaddrinfo(addrResult); // освобождение памяти для addrResult
	WSACleanup();//доинициализируем библиотеку
	return 0;

}