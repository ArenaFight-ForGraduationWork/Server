// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
