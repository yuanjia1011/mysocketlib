#include "StdAfx.h"
#include "Server.h"
#include "FormClientSocket.h"

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
	
}

ISocket* CServer::AllocClientSocket( SOCKET ClientSocket )
{
	CFormClientSocket *pClientSocket= new CFormClientSocket(ClientSocket);
	return pClientSocket;
}
