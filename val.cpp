#include "val.h"

HANDLE IOCP;
Player players[1010];
Player boss;

vector<int> move_thread;
// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}



int GetNewClientID() {
	for (int i = 0; i < MAX_USER; ++i) {
		if (players[i].in_use) continue;
		return i;
	}
	cout << "MAX USER\n";
	exit(-1);
}
bool in_range(int a, int b)
{
	if (players[b].x - 3 <= players[a].x && players[b].x + 3 >= players[a].x) {
		if (players[b].z - 3 <= players[a].z && players[b].z + 3 >= players[a].z)
			return true;
	}
	return false;

	//return 5 * 4 >=
	//	(players[a].x - players[b].x)*(players[a].x - players[b].x) + (players[a].z - players[b].z)*(players[a].z - players[b].z);
}
void Send_Packet(void *packet, unsigned id) {
	network_info *over_ex = new network_info;
	int packet_size = reinterpret_cast<unsigned char *>(packet)[0];
	memcpy(over_ex->IOCPbuf, packet, packet_size);
	over_ex->io_type = IO_SEND;
	ZeroMemory(&over_ex->overlapped, sizeof(WSAOVERLAPPED));
	over_ex->wsabuf.buf = over_ex->IOCPbuf;
	over_ex->wsabuf.len = packet_size;
	unsigned long IOsize;
	WSASend(players[id].overlapped_ex.s, &over_ex->wsabuf, 1, &IOsize, NULL, &over_ex->overlapped, NULL);
}
void send_remove_player(int to, int id)
{
	remove_player remove_player_packet;
	remove_player_packet.id = players[id].id;
	remove_player_packet.size = sizeof(remove_player);
	remove_player_packet.type = REMOVE_PLAYER;
	Send_Packet(&remove_player_packet, to);
}



CRITICAL_SECTION timer_lock;
priority_queue<event_type, vector<event_type>, mycomparison> p_queue;

