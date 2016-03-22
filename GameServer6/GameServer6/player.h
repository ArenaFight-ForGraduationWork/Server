#pragma once
#include "stdafx.h"
using namespace std;

#define LOBBY 1
#define ROOM 2
#define MAX_BUFF_SIZE   4096
#define MAX_PACKET_SIZE  4096

struct network_info {
	WSAOVERLAPPED overlapped;
	SOCKET s;
	bool is_recv;
	WSABUF wsabuf;
	char IOCPbuf[MAX_BUFF_SIZE];
	char PacketBuf[MAX_PACKET_SIZE];
	int prev_data_size;
	int curr_packet_size;
};

class Player {
public:
	bool in_use;
	std::mutex vl_lock;
	network_info overlapped_ex;
	string in_room;
	int id;
	float x = 0, y = 0;
	int state;
	void state_change(int type) { state = type; }
	Player() {};
	~Player() {};
};