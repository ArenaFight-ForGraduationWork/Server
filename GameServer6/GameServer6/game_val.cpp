#include "stdafx.h"

Player players[MAX_USER];
RoomManager room_manager;

HANDLE IOCP;

void error_display(char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s", msg);
	wprintf(L"에러[%s]\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}


void Init_Player(int id)
{
	players[id].overlapped_ex.curr_packet_size = 0;
	players[id].overlapped_ex.is_recv = true;
	ZeroMemory(&players[id].overlapped_ex.overlapped, sizeof(WSAOVERLAPPED));
	players[id].overlapped_ex.prev_data_size = 0;
	players[id].overlapped_ex.wsabuf.buf = players[id].overlapped_ex.IOCPbuf;
	players[id].overlapped_ex.wsabuf.len = MAX_BUFF_SIZE;
}

void ShutDown_Server()
{
	WSACleanup();
}

void Init_Server()
{
	WSADATA wsadata;

	_wsetlocale(LC_ALL, L"korean");
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	for (int i = 0; i < MAX_USER; ++i) {
		players[i].in_use = false;
		Init_Player(i);
	}
}
void Send_Packet(void *packet, unsigned id)
{
	network_info *over_ex = new network_info;
	int packet_size = reinterpret_cast<char *>(packet)[0];
	memcpy(over_ex->IOCPbuf, packet, packet_size);
	over_ex->is_recv = false;
	ZeroMemory(&over_ex->overlapped, sizeof(WSAOVERLAPPED));
	over_ex->wsabuf.buf = over_ex->IOCPbuf;
	over_ex->wsabuf.len = packet_size;
	unsigned long IOsize;
	WSASend(players[id].overlapped_ex.s, &over_ex->wsabuf, 1, &IOsize, NULL, &over_ex->overlapped, NULL);
}

void Send_Pos_Packet(void *packet, unsigned id)
{
	packet_pos  pos_packet;

	pos_packet.id = id;
	pos_packet.size = sizeof(packet_pos);
	pos_packet.type = SC_POS;
	pos_packet.x = players[id].x;
	pos_packet.y = players[id].y;
	Send_Packet(&pos_packet, id);
}

void Process_Packet(char *packet, unsigned id)
{
	switch (packet[1]) {
	case CREATE_ROOM:
	{
						create_room *create_room_packet = reinterpret_cast<create_room *>(packet);
						cout << "create_room" << endl;
						printf("roomname:%s roomstage:%d id:%d\n", create_room_packet->room_name, create_room_packet->stage, create_room_packet->id);
						room_manager.RoomCreate(create_room_packet->room_name, create_room_packet->stage, create_room_packet->id);
						break;
	}
	case ALL_USER_VIEW:
	{
						  break;
	}
	case SC_PUT_PLAYER:
	{
						  break;
	}
	case JOIN_ROOM:
	{
					  join_room *join_room_packet = reinterpret_cast<join_room *>(packet);
					  cout << "join_room" << endl;
					  printf("roomname:%s user_id:%d\n", join_room_packet->room_name, join_room_packet->id);
					  for (int i = 0; i < room_manager.RoomVector.size(); ++i) {
						  if (room_manager.RoomVector[i].getName() == join_room_packet->room_name) {
							  room_manager.join_room(join_room_packet->room_name, join_room_packet->id);
							  break;
						  }
					  }
					  break;
	}
	case UP:
	{
			   cout << "UP" << endl;
			   players[id].y++;
			   printf("id:%d x:%.0f y:%.0f\n", players[id].id, players[id].x, players[id].y);
			   players[id].vl_lock.lock();
			   for (int i = 0; i < room_manager.RoomVector.size(); ++i) {
				   if (room_manager.RoomVector[i].getName() == players[id].in_room) {
					   room_manager.RoomVector[i].send_pos_packet(players[id].id);
				   }
			   }
			   players[id].vl_lock.unlock();
			   break;
	}
	case DOWN:
	{
				 cout << "DOWN" << endl;
				 if (players[id].y > 0)
					 players[id].y--;
				 printf("id:%d x:%.0f y:%.0f\n", players[id].id, players[id].x, players[id].y);
				 players[id].vl_lock.lock();
				 for (int i = 0; i < room_manager.RoomVector.size(); ++i) {
					 if (room_manager.RoomVector[i].getName() == players[id].in_room) {
						 room_manager.RoomVector[i].send_pos_packet(players[id].id);
					 }
				 }
				 players[id].vl_lock.unlock();
			   break;
	}
	case LEFT:
	{
				 cout << "LEFT" << endl;

				 if (players[id].x > 0)
					 players[id].x--;
				 printf("id:%d x:%.0f y:%.0f\n", players[id].id, players[id].x, players[id].y);
				 players[id].vl_lock.lock();
				 for (int i = 0; i < room_manager.RoomVector.size(); ++i) {
					 if (room_manager.RoomVector[i].getName() == players[id].in_room) {
						 room_manager.RoomVector[i].send_pos_packet(players[id].id);
					 }
				 }
				 players[id].vl_lock.unlock();
			   break;
	}
	case RIGHT:
	{
				  cout << "RIGHT" << endl;
				  players[id].x++;
				  printf("id:%d x:%.0f y:%.0f\n", players[id].id, players[id].x, players[id].y);
				  players[id].vl_lock.lock();
				  for (int i = 0; i < room_manager.RoomVector.size(); ++i) {
					  if (room_manager.RoomVector[i].getName() == players[id].in_room) {
						  room_manager.RoomVector[i].send_pos_packet(players[id].id);
					  }
				  }
				  players[id].vl_lock.unlock();
			   break;
	}
	default:
		printf("Invalid Packet Received from Client ID:%d\n", id);
		exit(-1);
	}
}

void Worker_Thread()
{
	DWORD IOsize;
	ULONG key;
	network_info *over_ex;
	while (true){
		GetQueuedCompletionStatus(IOCP, &IOsize, &key,
			reinterpret_cast<LPOVERLAPPED *>(&over_ex),
			INFINITE);
		// ERROR 처리
		// 접속 종료 처리
		if (0 == IOsize) {
			packet_remove_player rem_player;
			rem_player.id = key;
			rem_player.size = sizeof(rem_player);
			rem_player.type = SC_REMOVE_PLAYER;
			players[key].vl_lock.lock();
			players[key].vl_lock.unlock();
			players[key].in_use = false;
			cout << "종료하는 유저 id : " << rem_player.id << endl;
			closesocket(over_ex->s);
			continue;
		}
		if (over_ex->is_recv == true) {
			// 페킷조립 및 실행
			int data_to_process = IOsize;
			char * buf = over_ex->IOCPbuf;
			while (0 < data_to_process) {
				if (0 == over_ex->curr_packet_size)
					over_ex->curr_packet_size = buf[0];
				int need_to_build =
					over_ex->curr_packet_size
					- over_ex->prev_data_size;
				if (need_to_build <= data_to_process) {
					// 패킷 조립
					memcpy(over_ex->PacketBuf + over_ex->prev_data_size,
						buf, need_to_build);
					Process_Packet(over_ex->PacketBuf, key);
					over_ex->curr_packet_size = 0;
					over_ex->prev_data_size = 0;
					data_to_process -= need_to_build;
					buf += need_to_build;
				}
				else {
					// 훗날을 기약
					memcpy(over_ex->PacketBuf + over_ex->prev_data_size,
						buf, data_to_process);
					over_ex->prev_data_size += data_to_process;
					data_to_process = 0;
					buf += data_to_process;
				}
			}
			// 다시 RECV
			unsigned long recv_flag = 0;
			WSARecv(over_ex->s, &over_ex->wsabuf, 1, NULL,
				&recv_flag, &over_ex->overlapped, NULL);
		}
		else {
			delete over_ex;
		}
	}
}

int GetNewClientID()
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (players[i].in_use) continue;
		cout << "new" << endl;
		return i;
	}
	cout << "MAX USER FULL\n";
	exit(-1);
}
void Accept_Thread()
{
	sockaddr_in accept_addr;
	sockaddr_in client_addr;

	SOCKET accept_socket = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&accept_addr, sizeof(accept_addr));
	accept_addr.sin_family = AF_INET;
	accept_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	accept_addr.sin_port = htons(SERVER_PORT);

	int err = ::bind(accept_socket,
		reinterpret_cast<sockaddr *>(&accept_addr),
		sizeof(accept_addr));
	if (SOCKET_ERROR == err) {
		error_display("BIND:", WSAGetLastError());
		exit(-1);
	}
	err = listen(accept_socket, 10);
	if (SOCKET_ERROR == err) {
		error_display("LISTEN:", WSAGetLastError());
		exit(-1);
	}
	while (true)
	{
		int addr_len = sizeof(accept_addr);
		SOCKET new_socket = WSAAccept(accept_socket,
			reinterpret_cast<sockaddr *>(&client_addr),
			&addr_len, NULL, NULL);
		if (INVALID_SOCKET == new_socket) {
			error_display("ACCEPT:", WSAGetLastError());
			exit(-1);
		}
		int id = GetNewClientID();
		Init_Player(id);
		players[id].id = id;
		players[id].overlapped_ex.s = new_socket;
		players[id].state = LOBBY;
		CreateIoCompletionPort(
			reinterpret_cast<HANDLE>(new_socket),
			IOCP, id, 0);

		unsigned long recv_flag = 0;
		err = WSARecv(new_socket,
			&players[id].overlapped_ex.wsabuf, 1,
			NULL, &recv_flag,
			reinterpret_cast<LPOVERLAPPED>(&players[id].overlapped_ex), NULL);
		if (SOCKET_ERROR == err) {
			int err_code = WSAGetLastError();
			if (WSA_IO_PENDING != err_code) {
				error_display("ACCEPT(WSARecv):", WSAGetLastError());
				exit(-1);
			}
		}

		packet_put_player pp_packet;
		pp_packet.id = players[id].id;
		pp_packet.size = sizeof(pp_packet);
		pp_packet.type = SC_PUT_PLAYER;

		// 새 유저 왔다고 전체 유저한테 알림
		for (int i = 0; i < MAX_USER; ++i){
			if (players[i].in_use == true && players[i].state == LOBBY)
				Send_Packet(&pp_packet, i);
		}

		players[id].in_use = true; // 지금 접속한 유저 상태 변경

		// 새 유저한테 현재 접속한 유저 목록 알림
		all_user_view user_view_packet;
		user_view_packet.type = ALL_USER_VIEW;
		user_view_packet.size = sizeof(user_view_packet);
		user_view_packet.yourid = players[id].id;
		int count = 0;
		for (int i = 0; i < 100; ++i) {
			if (players[i].in_use == true && players[i].state == LOBBY) {
				count++;
				user_view_packet.id[i] = players[i].id;
			}
		}
		user_view_packet.count = count;
		Send_Packet(&user_view_packet, players[id].id);


		/*
		for (int i = 0; i < MAX_USER; ++i){
		if (false == players[i].in_use) continue;
		if (i == id) continue;

		pp_packet.id = i;
		players[id].vl_lock.lock();
		players[id].vl_lock.unlock();
		Send_Packet(&pp_packet, id);
		}
		*/
	}
}
