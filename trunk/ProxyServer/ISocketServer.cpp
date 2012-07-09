#include "StdAfx.h"
#include "ISocketServer.h"
#include "IOCP.h"
#include "ISocketClient.h"

ISocketServer::ISocketServer(void)
{
}


ISocketServer::~ISocketServer(void)
{
}

bool ISocketServer::StartListen( short sPort,bool bSharePort /*= false*/ )
{
	sockaddr_in  service;  
	service.sin_family=AF_INET;  
	service.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	service.sin_port = htons(sPort);  
	m_sServerPort = sPort;
	if(bind(m_Socket,(SOCKADDR*)&service,sizeof(service)) == SOCKET_ERROR)  
	{  
		Close();  
		return  false;  
	} 

	if(listen(m_Socket,20) == SOCKET_ERROR) 
	{
		Close();
		return false;
	}else
	{
		CloseHandle((HANDLE)_beginthreadex(0,0,(unsigned int (__stdcall *)(void *))&AcceptThread,this,0,0));
		return true;
	}
}

DWORD WINAPI ISocketServer::AcceptThread( LPVOID pParam )
{
	ISocketServer *pServerSocket = (ISocketServer *)pParam;
	if(NULL == pServerSocket)
		return 1;
	pServerSocket->OnStartListen();
	while (TRUE)
	{
		SOCKADDR_IN addrAccept;
		ZeroMemory(&addrAccept, sizeof(SOCKADDR_IN));
		int nSockLen = sizeof(SOCKADDR);
		SOCKET sockAccept = WSAAccept(pServerSocket->GetSocket(), (SOCKADDR*)&addrAccept, &nSockLen, 0, 0);
		if(INVALID_SOCKET == sockAccept)
		{
			printf("WSAAccept errors!\n");
			return 0;
		}
		
		ISocket *pSocketclient = pServerSocket->AllocClientSocket(sockAccept);
		pSocketclient->SetStatus(SOCKET_STATUS_CONNEECTED);
		pSocketclient->RegToIOCP();
		pSocketclient->SyncRecv(1024);
		pServerSocket->OnAccept(pSocketclient);
		
	}
	return 0;
}

unsigned int	WINAPI ISocketServer::WorkerThread( LPVOID CompletionPortID )
{
	return 0;
}

