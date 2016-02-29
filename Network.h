#include "stdafx.h"

#define MAX_IOCP_THREAD		32


class Network {
private:
	char ip[16];
	int port;
	int workerThreadCount;
	SOCKET listenSocket;
	HANDLE iocp;
	thread *acceptThread;
	array<thread *, 32> workerThread;
	static DWORD WINAPI AcceptThread(LPVOID serverPtr);
	static DWORD WINAPI WorkerThread(LPVOID serverPtr);
	bool createListenSocket();
	
public:
	Network();
	~Network();
	void initialize();
	bool run();

	

	SOCKET	ListenSocket();
	HANDLE	Iocp();
	void onAccept(SOCKET accepter, SOCKADDR_IN addrinfo);

};


