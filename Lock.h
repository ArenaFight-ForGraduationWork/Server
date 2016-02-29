#pragma once
#include "stdafx.h"
class Lock
{
private:
	recursive_mutex mutex;
	wstring	name;
	size_t lockId;
	size_t threadId;

	wstring lockingFile;
	int lockingLine;

public:
	Lock(WCHAR* name);
	virtual ~Lock();

	const WCHAR* Name();
	size_t LockId();

	recursive_mutex& Mutex();
	void lock(LPCWSTR fileName, int lineNo);
	void unlock();

	void SetThreadId(size_t id);
	size_t ThreadId();
};

class LockSafeScope
{
	Lock  *lock;
public:
	LockSafeScope(Lock *lock, LPCWSTR fileName, int lineNo);
	~LockSafeScope();
};

#define SAFE_LOCK(lock)     LockSafeScope __lockSafe(&lock, _W(__FILE__), __LINE__);

// 데드락 탐지를 위함
class LockManager : public Singleton < LockManager >
{
	size_t idSeed;

public:
	LockManager();

	size_t generalId();
};
