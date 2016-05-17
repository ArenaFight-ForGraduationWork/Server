#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <mutex>
#include <set>

#define MAX_BUFF_SIZE   4096
#define MAX_PACKET_SIZE  4096

#define MAX_USER 1000

#define SERVER_PORT  4000

#define MAX_CHAT_SIZE  100


#define MAX_USER 1000
#define SERVER_PORT  4000
#define W			1
#define S			2
#define A			3
#define D			4

// 서버 -> 클라
#define LOGIN			1
#define PUT_PLAYER		2
#define PLAYER_POS		3
#define REMOVE_PLAYER   4
#define CREATE_ROOM_S   5
#define CREATE_ROOM_F   6
#define JOIN_ROOM_S		7
#define JOIN_ROOM_F		8
#define GAME_START_S    9
#define GAME_START_F    10
#define CHATTING	    11
#define ROOM_PUT_PLAYER 12
#define OUT_ROOM		13
#define BOSS_POS		14



// 클라 -> 서버
#define CREATE_ROOM		1
#define JOIN_ROOM		2
#define PLAYER_MOV      3
#define ROOM_EXIT		4
#define GAME_START		5
#define LOBBY_CHAT		6
#define ROOM_CHAT		7
#define PLAYER_MOV_END	8


#define LOBBY 1
#define ROOM  2
#define FIGHT 3

#define IO_SEND 1
#define IO_RECV 2
#define IO_NPC  3


#pragma pack (push, 1)
struct create_room {
	BYTE size;
	BYTE type;
	int id;
	//int room_name;
	char room_name[MAX_CHAT_SIZE];
	BYTE stage;
};
struct create_room_s {
	BYTE size;
	BYTE type;
	int id;
};
struct create_room_f {
	BYTE size;
	BYTE type;
	int id;
};
struct exit_room {
	BYTE size;
	BYTE type;
	int id;
};
struct join_room {
	BYTE size;
	BYTE type;
	int id;
	char room_name[MAX_CHAT_SIZE];
};
struct join_room_s {
	BYTE size;
	BYTE type;
	int id[4] = { -1, -1, -1, -1 };
};
struct join_room_f {
	BYTE size;
	BYTE type;
	int id;
};
struct out_room {
	BYTE size;
	BYTE type;
	int id = -1;
	int room_master;
};
struct game_start {
	BYTE size;
	BYTE type;
	int id;
};
struct game_start_s {
	BYTE size;
	BYTE type;
	int id[4] = { -1, -1, -1, -1 };
};
struct game_start_f {
	BYTE size;
	BYTE type;
};
struct login {
	BYTE size;
	BYTE type;
	int id[10];
	BYTE count;
	int yourid;
	float x[10];
	float z[10];
};
struct packet_player_move {
	BYTE size;
	BYTE type;
	BYTE move_type;
};
struct packet_chat {
	BYTE size;
	BYTE type;
	int id;
	char message[MAX_CHAT_SIZE];
};
//struct sc_packet_chat {
//	BYTE size;
//	BYTE type;
//	int id;
//	char message[MAX_CHAT_SIZE];
//};
struct player_position {
	BYTE size;
	BYTE type;
	int id;
	float x;
	float z;
	BYTE move_type;
};
struct put_player {
	BYTE size;
	BYTE type;
	int id;
	float x;
};
struct remove_player {
	BYTE size;
	BYTE type;
	int id;
};
#pragma pack (pop)