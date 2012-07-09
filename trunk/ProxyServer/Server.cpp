#include "StdAfx.h"
#include "Server.h"


CServer::CServer(void)
{
}


CServer::~CServer(void)
{
}

void CServer::OnStartListen()
{
	theConsole.Trace("CServer","OnStartListen");
}

void CServer::OnAccept( ISocket *pISocketPtr )
{
	theConsole.Trace("CServer","OnAccept");
	pISocketPtr->SyncRecv(1024);
}
