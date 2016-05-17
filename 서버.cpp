#pragma once
#include "protocol.h"
#include "val.h"
using namespace std;


void f() {
	for (int i = 0; i < 10; ++i) {
		if (players[i].in_use == true) {
			player_position pos_packet;
			pos_packet.size = sizeof(pos_packet);
			pos_packet.type = BOSS_POS;
			pos_packet.x = boss.x;
			pos_packet.z = boss.z;
			Send_Packet(&pos_packet, i);
		}
	}
}
void ai_thread() {
	while (1) {
		int s = rand() % 30;
		printf("s:%d\n", s);
		switch (rand() % 4 + 1) {
		case 1:
		{
			for (int i = 0; i < s; ++i) {
				if (boss.x > -1) {
					boss.x -= 10;
				}
				else
					boss.x = 0;
				Sleep(300);
				f();
			}
			break;
		}
		case 2:
		{
			for (int i = 0; i < s; ++i) {

				if (boss.x < 500) {
					boss.x += 10;
				}
				else
					boss.x = 499;
				Sleep(300);
				f();
			}
			break;
		}
		case 3:
		{
			for (int i = 0; i < s; ++i) {

				if (boss.z > -1) {
					boss.z -= 10;
				}
				else
					boss.z = 0;
				Sleep(300);
				f();
			}
			break;
		}
		case 4:
		{			
			for (int i = 0; i < s; ++i) {
				if (boss.z < 500) {
					boss.z += 10;
				}
				else
					boss.z = 499;
				Sleep(300);
				f();
			}
			break;
		}
		default:
			break;
		}
		Sleep(300);
	}
}

void player_move_thread() {
	while (1) {
		for (int i = 0; i < move_thread.size(); ++i) {
			if (players[move_thread[i]].time <= GetTickCount()) {
				if (players[move_thread[i]].dir == W) {
					players[move_thread[i]].z -= 100 * 0.1;
				}
				else if (players[move_thread[i]].dir == S) {
					players[move_thread[i]].z += 100 * 0.1;
				}
				else if (players[move_thread[i]].dir == A) {
					players[move_thread[i]].x -= 100 * 0.1;
				}
				else if (players[move_thread[i]].dir == D) {
					players[move_thread[i]].x += 100 * 0.1;
				}

				player_position pos_packet;

				pos_packet.size = sizeof(pos_packet);
				pos_packet.type = PLAYER_POS;
				pos_packet.x = players[move_thread[i]].x;
				pos_packet.z = players[move_thread[i]].z;
				pos_packet.id = move_thread[i];
				pos_packet.move_type = players[move_thread[i]].dir;


				Send_Packet(&pos_packet, move_thread[i]);


				for (int j = 0; j < 10; ++j) {
					if (players[j].in_use == true && players[j].id != move_thread[i])
						Send_Packet(&pos_packet, j);
				}

				printf("players[move_thread[i]].time:%d, GetTickCount():%d\n", players[move_thread[i]].time, GetTickCount());
				players[move_thread[i]].time = GetTickCount() + 100;
				//move_thread.erase(move_thread.begin() + i);
			}
		}
		Sleep(1);
	}
}
void Process_Packet (char *packet, unsigned id) {
	switch (packet[1]) {
	case PLAYER_MOV:
	{
		packet_player_move *player_move_packet = reinterpret_cast<packet_player_move *>(packet);
		move_thread.push_back(id);
		players[id].time = GetTickCount();
		players[id].dir = player_move_packet->move_type;
		break;
	}
	case PLAYER_MOV_END:
	{
		packet_player_move *player_move_packet = reinterpret_cast<packet_player_move *>(packet);
		for (int i = 0; i < move_thread.size(); ++i) {
			if (move_thread[i] == id) {
				printf("move_thread[i]:%d 무브끝\n", move_thread[i]);
				move_thread.erase(move_thread.begin() + i);
			}
		}
		break;
	}
		default:
			printf ("Invalid Packet Received from Client ID:%d\n", id);
			exit (-1);
	}
		
}
void Worker_Thread () {
	DWORD IOsize;
	ULONG key;
	network_info *over_ex;
	while (true) {
		GetQueuedCompletionStatus (IOCP, &IOsize, &key, reinterpret_cast<LPOVERLAPPED *>(&over_ex), INFINITE);
		// ERROR 처리
		// 접속 종료 처리
		if (0 == IOsize) {
			/*another_disconnect logout_player;
			logout_player.id = key;
			logout_player.size = sizeof (logout_player);
			logout_player.type = ANOTHER_DISCONNECT;
			players[key].in_use = false;
			players[key].locking.lock ();

			players[key].view_list.clear();

			players[key].locking.unlock();
			for (int i = 0; i < MAX_USER; ++i) {
				if (players[i].in_use == true) {
					Send_Packet (&logout_player, i);
				}
			}
			players[key].x = 5;
			players[key].z = 5;*/
			closesocket (over_ex->s);
			continue;
		}
		if (over_ex->io_type == IO_RECV) {
			int data_to_process = IOsize;
			char * buf = over_ex->IOCPbuf;
			while (0 < data_to_process) {
				if (0 == over_ex->curr_packet_size)
					over_ex->curr_packet_size = buf[0];
				int need_to_build = over_ex->curr_packet_size - over_ex->prev_data_size;
				if (need_to_build <= data_to_process) {
					memcpy (over_ex->PacketBuf + over_ex->prev_data_size, buf, need_to_build);
					Process_Packet (over_ex->PacketBuf, key);
					over_ex->curr_packet_size = 0;
					over_ex->prev_data_size = 0;
					data_to_process -= need_to_build;
					buf += need_to_build;
				}
				else {
					memcpy (over_ex->PacketBuf + over_ex->prev_data_size, buf, data_to_process);
					over_ex->prev_data_size += data_to_process;
					data_to_process = 0;
					buf += data_to_process;
				}
			}
			unsigned long recv_flag = 0;
			WSARecv (over_ex->s, &over_ex->wsabuf, 1, NULL, &recv_flag, &over_ex->overlapped, NULL);
		}
		else if(over_ex->io_type == IO_SEND) {
			delete over_ex;
		}
		else if (over_ex->io_type == IO_NPC) {
			
		}
	}
}

void Accept_Thread () {
	sockaddr_in accept_addr;
	sockaddr_in client_addr;

	SOCKET accept_socket = WSASocket (AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory (&accept_addr, sizeof (accept_addr));
	accept_addr.sin_family = AF_INET;
	accept_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	accept_addr.sin_port = htons (SERVER_PORT);

	int err = ::bind (accept_socket, reinterpret_cast<sockaddr *>(&accept_addr), sizeof (accept_addr));
	if (SOCKET_ERROR == err) {
		err_display ("bind()");
		exit (-1);
	}
	err = listen (accept_socket, 10);
	if (SOCKET_ERROR == err) {
		err_display ("listen()");
		exit (-1);
	}
	while (true) {
		int addr_len = sizeof (accept_addr);
		SOCKET new_socket = WSAAccept (accept_socket, reinterpret_cast<sockaddr *>(&client_addr), &addr_len, NULL, NULL);
		if (INVALID_SOCKET == new_socket) {
			err_display ("accept()");
			exit (-1);
		}
		int id = GetNewClientID ();
		players[id].overlapped_ex.curr_packet_size = 0;
		players[id].overlapped_ex.io_type = IO_RECV;
		ZeroMemory (&players[id].overlapped_ex.overlapped, sizeof (WSAOVERLAPPED));
		players[id].overlapped_ex.prev_data_size = 0;
		players[id].overlapped_ex.wsabuf.buf = players[id].overlapped_ex.IOCPbuf;
		players[id].overlapped_ex.wsabuf.len = MAX_PACKET_SIZE;
		players[id].id = id;
		players[id].overlapped_ex.s = new_socket;
		players[id].x = 5;
		players[id].z = 5;
		CreateIoCompletionPort (reinterpret_cast<HANDLE>(new_socket), IOCP, id, 0);

		unsigned long recv_flag = 0;
		err = WSARecv (new_socket, &players[id].overlapped_ex.wsabuf, 1, NULL, &recv_flag, reinterpret_cast<LPOVERLAPPED>(&players[id].overlapped_ex), NULL);
		if (SOCKET_ERROR == err) {
			int err_code = WSAGetLastError ();
			if (WSA_IO_PENDING != err_code) {
				err_display ("accept()");
				exit (-1);
			}
		}
		
		player_position pp_packet;
		pp_packet.id = players[id].id;
		pp_packet.size = sizeof(pp_packet);
		pp_packet.type = PUT_PLAYER;
		pp_packet.x = players[id].x;
		pp_packet.z = players[id].z;


		// 새 유저 왔다고 전체 유저한테 알림
		for (int i = 0; i < MAX_USER; ++i) {
			if (players[i].in_use == true)
				Send_Packet(&pp_packet, i);
		}

		players[id].in_use = true; // 지금 접속한 유저 상태 변경
								   // 새 유저한테 현재 접속한 유저 목록 알림
		login login_packet;
		login_packet.type = LOGIN;
		login_packet.size = sizeof(login_packet);
		login_packet.yourid = players[id].id;

		int count = -1;
		for (int i = 0; i < 10; ++i) {
			if (players[i].in_use == true) {
				count++;
				login_packet.id[i] = players[i].id;
				login_packet.x[i] = players[i].x;
				login_packet.z[i] = players[i].z;
			}
		}
		login_packet.count = count;
		printf("login_packet.count:%d\n", login_packet.count);
		Send_Packet(&login_packet, players[id].id);

	}
}


int main (int argc, char *argv[]) {
	const auto THREADS = 6;

	vector <thread *> worker_threads;

	WSADATA wsadata;

	WSAStartup (MAKEWORD (2, 2), &wsadata);

	for (int i = 0; i < MAX_USER; ++i) {
		players[i].in_use = false;
		players[i].overlapped_ex.curr_packet_size = 0;
		players[i].overlapped_ex.io_type = IO_RECV;
		ZeroMemory (&players[i].overlapped_ex.overlapped, sizeof (WSAOVERLAPPED));
		players[i].overlapped_ex.prev_data_size = 0;
		players[i].overlapped_ex.wsabuf.buf = players[i].overlapped_ex.IOCPbuf;
		players[i].overlapped_ex.wsabuf.len = MAX_PACKET_SIZE;
	}
	boss.x = 50;
	boss.z = 50;
	IOCP = CreateIoCompletionPort (INVALID_HANDLE_VALUE, NULL, NULL, 0);
	for (int i = 0; i < THREADS; ++i)
		worker_threads.push_back (new thread{Worker_Thread});
	auto accept_thread = thread{Accept_Thread};
	auto Move_thread = thread{ player_move_thread };
	auto boss_thread = thread{ ai_thread };
	for (auto t : worker_threads) t->join ();
	accept_thread.join ();
	Move_thread.join();
	boss_thread.join();
	return 0;
}