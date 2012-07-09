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
	service.sin_addr.s_addr=inet_addr("0.0.0.0");  
	service.sin_port=htons(sPort);  
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
	sockaddr  RemoteClientAddr; 
	int addLen = sizeof(RemoteClientAddr);
	while (TRUE)
	{
		SOCKET sock = WSAAccept(pServerSocket->GetSocket(),&RemoteClientAddr,&addLen,NULL,NULL);
		ISocketClient *pSocketclient = new ISocketClient(sock);
		pSocketclient->BindToIOCP();
		pServerSocket->OnAccept(pSocketclient);
		
	}
	return 0;
}
