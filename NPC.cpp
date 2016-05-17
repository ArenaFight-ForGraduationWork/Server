#include <WinSock2.h>
#include <Windows.h>

#include <queue>
#include <set>


using namespace std;

#include "protocol.h"
#include "val.h"



void add_timer(int obj_id, int m_sec, int event_type, int caller_id)
{
	struct event_type event_object = { obj_id, m_sec + GetTickCount(), event_type, caller_id };
	//printf("add_timer(obj_id=%d,m_sec=%d,event_type=%d,caller_id=%d)\n", obj_id, m_sec, event_type, caller_id);
	//printf("add_timer() wakeup_time = %d\n", event_object.wakeup_time );
	EnterCriticalSection(&timer_lock);
	p_queue.push(event_object);
	LeaveCriticalSection(&timer_lock);
}



void Timer_Thread()
{
	while (true) {
		Sleep(10);
		EnterCriticalSection(&timer_lock);
		while (!p_queue.empty()) {
			struct event_type top_object = p_queue.top();
			if (top_object.wakeup_time > GetTickCount()) {
				//printf("top_object.caller_id:%d, GetTickCount():%d, top_object.wakeup_time:%d\n", top_object.caller_id, GetTickCount(), top_object.wakeup_time);
				break;
			}
			p_queue.pop();
			network_info *overlapped = new network_info;
			overlapped->io_type = top_object.event_id;
			overlapped->iocp_caller_id = top_object.caller_id;
			ZeroMemory(&overlapped->overlapped, sizeof(overlapped->overlapped));
			PostQueuedCompletionStatus(IOCP, 1, top_object.obj_id,
				&overlapped->overlapped);
		}
		LeaveCriticalSection(&timer_lock);
	}
}


void Create_Monster()
{
}

void npc_move() {
	while (1) {
		for (int i = 10; i < 1010; ++i) {
			Sleep(1);
			add_timer(i, 100, IO_NPC, i);
		}
	}
}


void npc_init()
{
	//InitializeCriticalSection(&timer_lock);
	//auto timer_thread = thread{ Timer_Thread };
	//for (int i = 0; i < 1000; ++i)
	//	Create_Monster();
	//auto npc_thread = thread{ npc_move };
	//auto npc_thread = thread{ ai_thread };
	//timer_thread.join();
	//npc_thread.join();
}