#include "stdafx.h"
#include "SessionManager.h"

SessionManager::SessionManager()
: lock(L"SessionManager")
{
	idSeed = 1;
	maxConnection = SESSION_CAPACITY;
}

SessionManager::~SessionManager()
{
	vector<Session *> removeSessionVec;
	removeSessionVec.resize(sessionList_.size());
	std::copy(sessionList_.begin(), sessionList_.end(), removeSessionVec.begin());
	for (auto session : removeSessionVec) {
		session->onClose();
	}
	sessionList_.clear();
}

list<Session*>& SessionManager::sessionList()
{
	return sessionList_;
}

UINT SessionManager::createSessionId()
{
	return idSeed++;
}

bool SessionManager::addSession(Session *session)
{
	//SAFE_LOCK(lock);
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end()) {
		return false;
	}
	if (sessionCount > maxConnection) {
		cout << "session full : " << sessionCount << endl;
		return false;
	}

	session->setId(this->createSessionId());
	sessionList_.push_back(session);
	++sessionCount;
	return true;
}

//소켓을 닫으라는 클라이언트에게 보냅니다.
bool SessionManager::closeSession(Session *session)
{
	//SAFE_LOCK(lock_);
	if (session == nullptr) {
		return false;
	}
	auto findSession = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (findSession != sessionList_.end()) {
		Session *delSession = *findSession;
		printf("detected close by client [%S]", delSession->clientAddress().c_str());
		::closesocket(delSession->socket());

		sessionList_.remove(delSession);
		--sessionCount;
		return true;
	}
	return false;
}

//소켓을 강제로 닫습니다.
void SessionManager::forceCloseSession(Session *session)
{
	//SAFE_LOCK(lock_);
	if (!session) {
		return;
	}

	//우아한 종료 유도. 원래는 클라이언트에서 서버 접속을 종료하도록 유도해야 한다.
	LINGER linger;
	linger.l_onoff = 1;   //사용
	linger.l_linger = 0;  //대기시간, 0일시 완료 안된 패킷 버리고 즉시 종료.

	::setsockopt(session->socket(), SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof(linger));
	this->closeSession(session);
}

Session* SessionManager::session(UINT id)
{
	//SAFE_LOCK(lock_);
	Session *findSession = nullptr;

	for (auto session : sessionList_) {
		if (session->Id() == id) {
			findSession = session;
			break;
		}
	}

	return findSession;
}
