#pragma once
#include "stdafx.h"
#include "room.h"


Room::Room(string name, int stage, int id) : room_name(name), stage(stage) {
	cout << "룸생성 방제:" << name << " 스테이지:" << stage << endl;
	player.push_back(id);
	players[id].state_change(ROOM);
	players[id].in_room = name;
}
Room::~Room() {}

void Room::join_user(string name, int id) {
	player.push_back(id);
	players[id].state_change(ROOM);
	cout << "id:" << players[id].id << "조인완료" << endl;
	players[id].in_room = name;
};

void Room::send_pos_packet(int id) {
	packet_pos  pos_packet;

	pos_packet.size = sizeof(packet_pos);
	pos_packet.type = POS;
	pos_packet.x = players[id].x;
	pos_packet.y = players[id].y;
	pos_packet.id = player[id];
	for (int i = 0; i < player.size(); ++i) {
		
		Send_Packet(&pos_packet, player[i]);
	}
}