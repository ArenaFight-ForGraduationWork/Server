// GameServer6.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


const auto NUM_THREADS = 6;


int _tmain(int argc, _TCHAR* argv[])
{
	vector <thread *> worker_threads;

	Init_Server();
	IOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	for (int i = 0; i < NUM_THREADS; ++i)
		worker_threads.push_back(new thread{ Worker_Thread });
	auto accept_thread = thread{ Accept_Thread };
	while (true) {
		Sleep(1000);
		// if (g_server_shutdown == false) break;
	}
	for (auto t : worker_threads) t->join();
	accept_thread.join();
	ShutDown_Server();
	return 0;
}

