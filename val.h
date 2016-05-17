#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <set>
#include <vector>
#include <iostream>
#include <thread>
#include<time.h>
#include "queue"
#include "protocol.h"

struct network_info {
	WSAOVERLAPPED overlapped;
	SOCKET s;
	BYTE io_type;
	WSABUF wsabuf;
	char IOCPbuf[MAX_PACKET_SIZE];
	char PacketBuf[MAX_PACKET_SIZE];
	int prev_data_size;
	int curr_packet_size;
	WORD iocp_caller_id;
};

class Player {
public:
	bool in_use;
	std::mutex locking;
	network_info overlapped_ex;
	std::set<int> view_list;
	int id;
	float x = 5;
	float z = 5;
	int time = 0;
	BYTE dir;
};
using namespace std;
extern HANDLE IOCP;
extern Player players[1010];
extern Player boss;
extern vector<int> move_thread;
// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg);

// 소켓 함수 오류 출력
void err_display(char *msg);



int GetNewClientID();
bool in_range(int a, int b);
void Send_Packet(void *packet, unsigned id);
void send_remove_player(int to, int id);


struct event_type {
	int obj_id;
	unsigned wakeup_time;
	int event_id;
	int caller_id;
};


class mycomparison
{
	bool reverse;
public:
	mycomparison() {}
	bool operator() (const event_type lhs, const event_type rhs) const
	{
		return (lhs.wakeup_time > rhs.wakeup_time);
	}
};


extern CRITICAL_SECTION timer_lock;
extern priority_queue<event_type, vector<event_type>, mycomparison> p_queue;



void npc_init();