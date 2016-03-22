#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <set>
#include <mutex>
#include <string>
#include "protocol.h"
#include <conio.h>
using namespace std;

#define	BUF_SIZE				4096

WSABUF	send_wsabuf;
char 	send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUF_SIZE];
char	packet_buffer[BUF_SIZE];
DWORD	in_packet_size = 0;
int		saved_packet_size = 0;
int		myid;


void ReadPacket(SOCKET sock);

SOCKET sock;
Player player;

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
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
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}



// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0){
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

class EventData{
public:
	int value;

	void Set(int def) { value = def; }
};

struct network_info {
	WSAOVERLAPPED overlapped;
	bool is_recv;
	WSABUF wsabuf;
	char IOCPbuf[MAX_BUFF_SIZE];
	char PacketBuf[MAX_PACKET_SIZE];
	int prev_data_size;
	int curr_packet_size;
};

void Send_Packet(void *packet)
{
	network_info *over_ex = new network_info;
	int packet_size = reinterpret_cast<char *>(packet)[0];
	memcpy(over_ex->IOCPbuf, packet, packet_size);
	over_ex->is_recv = false;
	ZeroMemory(&over_ex->overlapped, sizeof(WSAOVERLAPPED));
	over_ex->wsabuf.buf = over_ex->IOCPbuf;
	over_ex->wsabuf.len = packet_size;
	unsigned long IOsize;
	WSASend(sock, &over_ex->wsabuf, 1,
		&IOsize, NULL, &over_ex->overlapped, NULL);
}


int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	char ch;
	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.1.3");
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUF_SIZE];
	int len;
	char room_name[100];
	int select = 0;
	EventData eventData;
	// 서버와 데이터 통신
	while(1) {
		ReadPacket(sock);
		cout << "1. 방생성 2. 방입장 4. 이동" << endl;
		scanf("%d", &select);
		switch (select)
		{
		case 1:
		{
				  create_room* p = reinterpret_cast<create_room*>(send_buffer);
				  p->size = sizeof(*p);
				  p->type = CREATE_ROOM;
				  p->id = myid;
				  cout << "방제입력:" << endl;
				  scanf("%s", &room_name);
				  strcpy(p->room_name, room_name);
				  cout << "스테이지 선택" << endl;
				  scanf("%d", &p->stage);
				  retval = send(sock, (char*)p, sizeof(*p), 0);
				  if (retval == SOCKET_ERROR)
					  printf("event data send ERROR\n");
				  ReadPacket(sock);
				  while (1) {
					  if (_kbhit()) {
						  ch = getch();
						  switch (tolower(ch)) {
						  case 'w':
						  {
									  printf("w\n");
									  packet_up* pp = reinterpret_cast<packet_up*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = UP;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }
						  case 's':
						  {
									  printf("s\n");
									  packet_down* pp = reinterpret_cast<packet_down*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = DOWN;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }
							 
						  case 'a':
						  {
									  packet_left* pp = reinterpret_cast<packet_left*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = LEFT;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");;

									  ReadPacket(sock);
									  break;
						  }
							  
						  case 'd':
						  {
									  printf("d\n");
									  packet_right* pp = reinterpret_cast<packet_right*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = RIGHT;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }
						  }
					  }
				  }
				  
				  break;
		}
		case 2:
		{
				  join_room* pp = reinterpret_cast<join_room*>(send_buffer);
				  pp->size = sizeof(*pp);
				  pp->type = JOIN_ROOM;
				  pp->id = myid;
				  cout << myid << endl;
				  cout << "입장할 방제목 입력:" << endl;
				  scanf("%s", &room_name);
				  strcpy(pp->room_name, room_name);
				  retval = send(sock, (char*)pp, sizeof(*pp), 0);
				  if (retval == SOCKET_ERROR)
					  printf("event data send ERROR\n");
				  ReadPacket(sock);
				  while (1) {
					  if (_kbhit()) {
						  ch = getch();
						  switch (tolower(ch)) {
						  case 'w':
						  {
									  printf("w\n");
									  packet_up* pp = reinterpret_cast<packet_up*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = UP;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }
						  case 's':
						  {
									  printf("s\n");
									  packet_down* pp = reinterpret_cast<packet_down*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = DOWN;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }

						  case 'a':
						  {
									  packet_left* pp = reinterpret_cast<packet_left*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = LEFT;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");;

									  ReadPacket(sock);
									  break;
						  }

						  case 'd':
						  {
									  printf("d\n");
									  packet_right* pp = reinterpret_cast<packet_right*>(send_buffer);
									  pp->size = sizeof(*pp);
									  pp->type = RIGHT;
									  retval = send(sock, (char*)pp, sizeof(*pp), 0);
									  if (retval == SOCKET_ERROR)
										  printf("event data send ERROR\n");
									  ReadPacket(sock);
									  break;
						  }
						  }
					  }
				  }
				  break;
		}
		case 3:
		{
				  break;
		}
		}
	
		/*
		sc_all_user_view view;
		retval = recvn(sock, (char*)&view, sizeof(sc_all_user_view), 0);
		if (retval == SOCKET_ERROR){
			err_display("recv()");
			return 0;
		}
		printf("%d %d\n", view.count, view.id);
		*/
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case CREATE_ROOM:
	{
					   	  break;
	}
	case ALL_USER_VIEW:
	{
						  all_user_view *my_packet = reinterpret_cast<all_user_view *>(ptr);
						  printf("my id:%d\n", my_packet->yourid);
						  myid = my_packet->yourid;
						  printf("all user count:%d\n", my_packet->count);
						  for (int i = 0; i < my_packet->count; ++i)
							  printf("id:%d\n", my_packet->id[i]);
						  break;
	}
	case SC_PUT_PLAYER:
	{
						  packet_put_player *my_packet = reinterpret_cast<packet_put_player *>(ptr);
						  printf("new user enter id:%d\n", my_packet->id);
						  break;
	}
	case POS:
	{
				packet_pos *my_packet = reinterpret_cast<packet_pos *>(ptr);
				player.x = my_packet->x;
				player.y = my_packet->y;
				printf("id:%d x:%d y:%d\n", my_packet->id, player.x, player.y);
				break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}


void ReadPacket(SOCKET sock)
{
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);
	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}