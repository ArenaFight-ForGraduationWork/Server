#pragma once
#include "stdafx.h"

class User {
	int id;
public:
	User(int id) : id(id) {};
	~User() {};
};

#define MAX_CHAT_SIZE  100

#pragma pack (push, 1)
struct create_room {
	BYTE size;
	BYTE type;
	BYTE id;
	//int room_name;
	char room_name[MAX_CHAT_SIZE];
	BYTE stage;
};
struct join_room {
	BYTE size;
	BYTE type;
	int id;
	char room_name[MAX_CHAT_SIZE];
};
struct all_user_view {
	BYTE size;
	BYTE type;
	BYTE id[100];
	BYTE count;
	BYTE yourid;
};
struct packet_up {
	BYTE size;
	BYTE type;
};

struct packet_down {
	BYTE size;
	BYTE type;
};

struct packet_left {
	BYTE size;
	BYTE type;
};

struct packet_right {
	BYTE size;
	BYTE type;
};

struct packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_CHAT_SIZE];
};

struct packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};

struct packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
};
struct packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};
#pragma pack (pop)