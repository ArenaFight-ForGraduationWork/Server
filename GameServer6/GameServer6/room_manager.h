#pragma once
#include "stdafx.h"

class RoomManager {
private:
	string room_name;
	int	MaxRoomCount = 100;
	int CurrentRoomCount = 0;
	
public:
	vector<Room> RoomVector;
	RoomManager();
	~RoomManager();
	void RoomCreate(string, int, int);
	void join_room(string, int);

};