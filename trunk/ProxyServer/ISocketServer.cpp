#include "StdAfx.h"
#include "ISocketServer.h"
#include "IOCP.h"
#include "ISocketClient.h"

IOCP *		ISocketServer::m_pIocpPtr = NULL;
DWORD		ISocketServer::m_dwServerNums = 0;
ISocketServer::ISocketServer(void)
{
	m_eSocketType = SOCKET_SERVER;
	m_dwServerNums ++;
	if(!m_pIocpPtr)
	{
		m_pIocpPtr = new IOCP;
		m_pIocpPtr->Init();
		m_pIocpPtr->StartWork(&WorkerThread);
	}
}


ISocketServer::~ISocketServer(void)
{
	m_dwServerNums --;
	if(m_dwServerNums == 0)
	{
		if(m_pIocpPtr)
		{
			ol.plPoint = this;
			ol.dwFlags = 0;
			ol.wsaBuf.buf = NULL;
			ol.wsaBuf.len = 0;
			ol.nOpCode = 0;
			m_pIocpPtr->StopWork(&ol.ol);
			delete m_pIocpPtr;m_pIocpPtr = NULL;
		}
	}
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
		SOCKET sockAccept = WSAAccept(pServerSocket->GetSocket(), (SOCKADDR*)&addrAccept ,  &nSockLen , 0, 0);
		if(INVALID_SOCKET == sockAccept)
		{
			printf("WSAAccept errors!\n");
			return 0;
		}
		theConsole.Trace("AcceptThread","接受客户端连接 %d.%d.%d.%d : %d",
			addrAccept.sin_addr.S_un.S_un_b.s_b1,
			addrAccept.sin_addr.S_un.S_un_b.s_b2,
			addrAccept.sin_addr.S_un.S_un_b.s_b3,
			addrAccept.sin_addr.S_un.S_un_b.s_b4,
			ntohs(addrAccept.sin_port));
		
		ISocketClient *pSocketclient = (ISocketClient *)(pServerSocket->AllocClientSocket(sockAccept));

		if(NULL == pServerSocket)
		{
			closesocket(sockAccept);
			sockAccept = SOCKET_INVALID;
			continue;
		}

		pSocketclient->SetStatus(SOCKET_STATUS_CONNEECTED);
		pServerSocket->RegToIOCP(pSocketclient);
		pSocketclient->SyncRecv(1024);
		pServerSocket->OnAccept(pSocketclient);
		
	}
	return 0;
}

UINT WINAPI ISocketServer::WorkerThread( LPVOID CompletionPortID )
{
	theConsole.Trace("IOCP","ServerWorkerThread %d",GetCurrentThreadId());
	HANDLE hCompletionPort = (HANDLE) CompletionPortID;
	void* re;
	DWORD berByte;
	OVERLAPPED* pOl = NULL;
	int nErrorCode = 0;
	while(TRUE)
	{
		BOOL bRet = GetQueuedCompletionStatus(hCompletionPort, &berByte, (LPDWORD)&re, (LPOVERLAPPED*)&pOl, INFINITE);
		ISocketServer *pMainSocket = (ISocketServer *)re;
		OVERLAPPEDPLUS* pOlp = (OVERLAPPEDPLUS*) pOl;
		ISocketClient *pISocket = (ISocketClient *)pOlp->plPoint;
		if(!bRet)
		{
			nErrorCode = WSAGetLastError();
			if (WSA_WAIT_TIMEOUT != WSAGetLastError())
			{
				pMainSocket->OnError(pISocket,nErrorCode);
			}
			continue;
		}

		if(0 == berByte)
		{
			if ( pOlp->nOpCode == OPCODE_STOP)
			{
				_endthreadex(0);
				return 0;
			}
			nErrorCode = WSAGetLastError();
			if( 0 != nErrorCode)
				pMainSocket->OnError(pISocket,nErrorCode);
			else
			{
				pMainSocket->OnClose(pISocket);
			}
			continue;
		}

		theConsole.Debug("WorkerThread","OperaCode:%d SendBytes:%d RecvBytes:%d",pOlp->nOpCode,pOlp->dwSendBytes,pOlp->dwRecvBytes);
		switch(pOlp->nOpCode)
		{
		case OPCODE_READ:
			pOlp->wsaBuf.buf[berByte] = '\0';
			pMainSocket->OnRecv(pISocket,pOlp->wsaBuf.buf,berByte);
			
			break;
		case OPCODE_WRITE:
			pMainSocket->OnSend(pISocket,pOlp->wsaBuf.buf,berByte);
			break;
		default:
			break;
		}
		if(pISocket->GetAlwaysRecv())
			pISocket->SyncRecv(1024);
	}
	_endthreadex(0);
	return TRUE;
}



void ISocketServer::OnRecv( ISocket *pISocketPtr,const TCHAR *pszBuff,size_t size )
{
	pISocketPtr->OnRecv(pszBuff,size);
}

void ISocketServer::OnSend( ISocket *pISocketPtr,const TCHAR *pszBuff,size_t size )
{
	pISocketPtr->OnSend(pszBuff,size);
}

void ISocketServer::OnClose( ISocket *pISocketPtr )
{
	pISocketPtr->OnClose();
	delete pISocketPtr;
}

BOOL ISocketServer::RegToIOCP( ISocket *pClientSocket )
{
	if(!m_pIocpPtr)
		return false;
	return m_pIocpPtr->BindIoCompletionPort((HANDLE)pClientSocket->GetSocket(),(ULONG_PTR)this);
}

void ISocketServer::OnError( ISocket *pISocketPtr,DWORD nErrorCode )
{
	pISocketPtr->onError(nErrorCode);
}

