#include "stdafx.h"
#include "Network.h"
Network::Network()
{
	cout << "Initialze network base" << endl;
	this->initialize();
}

Network::~Network()
{
	::closesocket(listenSocket);
	cout << "End network base" << endl;
}

void Network::initialize()
{
	//서버 설정
	strcpy(ip, "127.0.0.1");
	port = 9001;
	workerThreadCount = 5;
}

bool Network::createListenSocket()
{
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) {
		cout << "listenSocket fail" << endl;
		return false;
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int reUseAddr = 1;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reUseAddr, (int)sizeof(reUseAddr));

	int retval = ::bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		cout << "bind fail" << endl;
		return false;
	}

	retval = ::listen(listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		cout << "listen fail" << endl;
		return false;
	}
	cout << "server listen socket created, ip : " << ip << ", " << "port : " << port << endl;
	return true;
}

bool Network::run()
{
	if (MAX_IOCP_THREAD < workerThreadCount) {
		printf("workerThread limit[%d], but config setting [%d]", MAX_IOCP_THREAD, workerThreadCount);
		return false;
	}

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, workerThreadCount);
	if (iocp == nullptr) {
		return false;
	}
	this->createListenSocket();

	acceptThread = new thread((&Network::AcceptThread), this);

	for (int i = 0; i < workerThreadCount; ++i) {
		workerThread[i] = new thread((&Network::WorkerThread), this);
	}
	return true;
}

SOCKET Network::ListenSocket()
{
	return listenSocket;
}

HANDLE Network::Iocp()
{
	return iocp;
}


void Network::onAccept(SOCKET accepter, SOCKADDR_IN addrInfo)
{
	/*
	Session *session = new Session();
	if (session == nullptr) {
		cout << "accept session create fail" << endl;
		return;
	}
	//if (!session->onAccept(accepter, addrInfo)) {
	//	return;
	//}
	//if (!SessionManager::getInstance().addSession(session)) {
	//	return;
	//}
	//session->ioData[IO_READ].clear();

	HANDLE handle = CreateIoCompletionPort((HANDLE)accepter, this->Iocp(), (ULONG_PTR)&(*session), NULL);
	if (!handle) {
		return;
	}
	//cout << "client accecpt from[" << session->clientAddress().c_str() << "]" << endl;
	//session->recvStandBy();
	*/
}


DWORD WINAPI Network::AcceptThread(LPVOID serverPtr)
{
	/*
	Network	*server = (Network*)serverPtr;

	while (1) {
		SOCKET		acceptSocket = INVALID_SOCKET;
		SOCKADDR_IN recvAddr;
		static int addrLen = sizeof(recvAddr);
		acceptSocket = WSAAccept(server->ListenSocket(), (struct sockaddr *)&recvAddr, &addrLen, NULL, 0);
		if (acceptSocket == SOCKET_ERROR) {
			cout << "Accept fail" << endl;
			break;
		}
		server->onAccept(acceptSocket, recvAddr);
	}
	*/
	return 0;
}

DWORD WINAPI Network::WorkerThread(LPVOID serverPtr)
{
	/*
	Network *server;

	while (1) {
		IoData *ioData = nullptr;
		Session *session = nullptr;
		DWORD transferSize;

		BOOL ret = GetQueuedCompletionStatus(server->Iocp(), &transferSize, (PULONG_PTR)&session, (LPOVERLAPPED *)&ioData, INFINITE);
		if (!ret) {
			continue;
		}
		if (session == nullptr) {
			cout << "socket data broken" << endl;
			return 0;
		}
		if (transferSize == 0) {
			//SessionManager::getInstance().closeSession(session);
			continue;
		}

		switch (ioData->type()) {
		case IO_WRITE:
			continue;

		case IO_READ:
			continue;

		case IO_ERROR:
			continue;
		}
	}
	*/
	return 0;
}
