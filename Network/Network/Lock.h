#pragma once
#include <Windows.h>

class Lock
{
public:
	Lock(void)
	{
		InitializeCriticalSection(&m_CS);
	}

	virtual ~Lock(void)
	{
		DeleteCriticalSection(&m_CS);
	}

	void lock() { EnterCriticalSection(&m_CS); }
	void Unlock()
	{
		LeaveCriticalSection(&m_CS);
	}

protected:

private:
	CRITICAL_SECTION m_CS;
};

class LockHelper
{
public:
	LockHelper(Lock* pCS)
	{
		pCS->lock();
		m_pCS = pCS;
	}

	virtual ~LockHelper(void)
	{
		m_pCS->Unlock();
	}

protected:

private:
	Lock* m_pCS;
};