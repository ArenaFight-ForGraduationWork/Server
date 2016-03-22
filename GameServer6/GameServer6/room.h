#pragma once
using namespace std;


class Room {
private:
	string room_name;
	int room_id;
	int stage;
	vector<int> player;
public:
	Room(string, int, int);
	~Room();
	string getName() const { return room_name; };
	void join_user(string, int);
	void getAllUser() { 
		for (int i = 0; i < player.size(); ++i)
		{ 
			printf("룸안의 플레이어 id : %d\n", player[i]);
		}
	}
	void send_pos_packet(int);
};