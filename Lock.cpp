#include "stdafx.h"
#include "Lock.h"

#ifdef _DEBUG
#define STERN_MODE
#endif

const int INVALID_LINE = -1;

Lock::Lock(WCHAR *name)
{
	lockId = LockManager::getInstance().generalId();
	name = name;

	lockingFile.clear();
	lockingLine = INVALID_LINE;
}

Lock::~Lock()
{
	name.clear();
}

const WCHAR* Lock::Name()
{
	return name.c_str();
}

size_t Lock::LockId()
{
	return lockId;
}

recursive_mutex& Lock::Mutex()
{
	return mutex;
}

void Lock::lock(LPCWSTR fileName, int lineNo)
{
	mutex.lock();

	lockingFile = fileName;
	lockingLine = lineNo;
}

void Lock::unlock()
{
	mutex.unlock();

	lockingFile.clear();
	lockingLine = INVALID_LINE;
}

void Lock::SetThreadId(size_t id)
{
	threadId = id;
}

size_t Lock::ThreadId()
{
	return threadId;
}
//-------------------------------------------------------//
// �����ڿ��� ���� �ɰ�, ������ ���������� ��ü ������ ���� Ǭ��.
// std::lock_guard<lock_t> guard(lock) �� ����������, 
// ����� ������ ���� ���� �����Ѵ�.
LockSafeScope::LockSafeScope(Lock *lock, LPCWSTR fileName, int lineNo)
{
	if (lock == nullptr) {
		return;
	}
	lock = lock;

	lock->lock(fileName, lineNo);
	lock->SetThreadId(this_thread::get_id().hash());
}

LockSafeScope::~LockSafeScope()
{
	if (!lock) {
		return;
	}
	lock->unlock();
	lock->SetThreadId(0);
}

//-------------------------------------------------------//
LockManager::LockManager()
{
	idSeed = 0;
}


size_t LockManager::generalId()
{
	size_t id = idSeed++;
	return id;
}
