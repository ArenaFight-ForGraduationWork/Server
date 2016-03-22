#pragma once
#include "stdafx.h"
#include "room_manager.h"

RoomManager::RoomManager() {}
RoomManager::~RoomManager() {}
void RoomManager::RoomCreate(string name, int stage, int id) {
	if (CurrentRoomCount < 100) {
		RoomVector.push_back(Room(name, stage, id));
		CurrentRoomCount++;
	}
}
void RoomManager::join_room(string name, int id) {
	for (int i = 0; i < RoomVector.size(); ++i) {
		cout << RoomVector[i].getName() << endl;
		if (RoomVector[i].getName() == name) {
			RoomVector[i].join_user(name, id);
			RoomVector[i].getAllUser();
			break;
		}
	}
}