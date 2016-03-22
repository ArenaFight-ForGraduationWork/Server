#include <Windows.h>
#include <vector>
class User {
	int id;
public:
	User(int id) : id(id) {};
	~User() {};
};
#define MAX_BUFF_SIZE   4096
#define MAX_PACKET_SIZE  4096

#define MAX_USER 1000

#define SERVER_PORT  4000

#define MAX_CHAT_SIZE  100


#define CS_CHAT		5


#define CREATE_ROOM		1
#define ALL_USER_VIEW	2
#define SC_PUT_PLAYER	3
#define JOIN_ROOM		4
#define UP				5
#define DOWN			6
#define LEFT			7
#define RIGHT			8

#define POS           4
//#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_CHAT		4



#pragma pack (push, 1)
struct Player {
	int x{ 0 };
	int y{ 0 };
};
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