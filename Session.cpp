#include "stdafx.h"
#include "Session.h"


void Session::onClose()
{
	SessionManager::getInstance().closeSession(this);
}