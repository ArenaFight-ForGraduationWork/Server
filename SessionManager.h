#pragma once
#include "stdafx.h"
#include "Session.h"

#define SESSION_CAPACITY		(1000)

class SessionManager : public Singleton<SessionManager>
{
	typedef list<Session*> SessionList;

	SessionList	sessionList_;
	int sessionCount;
	int	maxConnection;
	Lock lock;
	UINT idSeed;


public:
	SessionManager();
	SessionManager(int maxConnection);
	~SessionManager();
	UINT				createSessionId();

	bool				addSession(Session *session);

	list<Session*>		&sessionList();
	bool				closeSession(Session *session);
	void				forceCloseSession(Session *session);

	Session				*session(UINT id);
};