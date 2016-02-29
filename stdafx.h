// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib,"ws2_32.lib")

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <thread>
#include <list>
#include <mutex>

#define SOCKET_BUF_SIZE 1024 * 10
using namespace std;
#include "Network.h"
#include "Session.h"
#include "SessionManager.h"
#include "Stream.h"
#include "Singleton.h"
#include "Lock.h"


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
