// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "ws2_32")

#include "targetver.h"

#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <set>
#include <string>
#include <mutex>
#include <stdio.h>
#include "player.h"
#include "room.h"
#include "room_manager.h"
#include "protocol.h"
#include <tchar.h>
#include "game_val.h"


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

using namespace std;



#define MAX_USER 1000
#define SERVER_PORT  4000

#define CS_CHAT		5
#define CS_CHAT2		6

#define CREATE_ROOM		1
#define ALL_USER_VIEW	2
#define SC_PUT_PLAYER	3
#define JOIN_ROOM		4
#define UP				5
#define DOWN			6
#define LEFT			7
#define RIGHT			8


#define POS				4


#define SC_POS           1
//#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_CHAT		4


