#include "StdAfx.h"
#include "ISocketClient.h"
#include "IOCP.h"
ClientList	ISocketClient::m_sVecClient;
HANDLE		ISocketClient::m_hMonitConnectThread = NULL;
DWORD		ISocketClient::m_dwClientNums = 0;
Mutex		ISocketClient::_mutex;
IOCP*		ISocketClient::m_pIocpPtr = NULL;
ISocketClient::ISocketClient(void):m_bAlwaysRecv(TRUE)
{
	m_dwClientNums ++;
	m_eSocketType = SOCKET_CLIENT;
	if(!m_pIocpPtr)
	{
		m_pIocpPtr = new IOCP;
		m_pIocpPtr->Init();
		m_pIocpPtr->StartWork(&WorkerThread);
	}
	//theConsole.Trace("ISocketClient","ClientNums %d", m_dwClientNums);
}

ISocketClient::ISocketClient( SOCKET socket ):ISocket(socket)
{
	ISocketClient();
}


ISocketClient::~ISocketClient(void)
{
	m_dwClientNums --;
	if(m_dwClientNums == 0)
	{
		if(m_hMonitConnectThread)
		{
			CloseHandle(m_hMonitConnectThread );
			m_hMonitConnectThread = NULL;
		}
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
	//theConsole.Trace("ISocketClient","ClientNums %d", m_dwClientNums);
}


void ISocketClient::SyncConnect( const TCHAR *pszRemoteAddress,short sPort )
{
	theConsole.Trace("ISocketClient","SyncConnect");
	SOCKADDR_IN RemoteAddInfo;
	RemoteAddInfo.sin_family = AF_INET;
	RemoteAddInfo.sin_port = htons(sPort);
	RemoteAddInfo.sin_addr.S_un.S_addr = inet_addr(pszRemoteAddress);
	strcpy_s(m_pszServerAddr,pszRemoteAddress);
	m_sServerPort = sPort;
	unsigned long ul = 1;
	ioctlsocket(m_Socket, FIONBIO, &ul); //设置为非阻塞模式
	int nResult = WSAConnect(m_Socket, (LPSOCKADDR)&RemoteAddInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL);
	if( SOCKET_ERROR == nResult)
	{
		int nErrorCode = GetErrorCode();
		if(nErrorCode != WSA_IO_PENDING && nErrorCode != 0 && nErrorCode != WSAEWOULDBLOCK)
		{
			onError(nErrorCode);
		}
	}
	AddToMoniter();
}
VOID ISocketClient::SyncSend(const CHAR * ptszMsg,size_t size )
{
	ZeroMemory(&ol,sizeof(ol));
	memcpy(ol.pBuf,ptszMsg,size);
	ol.plPoint = this;
	ol.dwFlags = 0;
	ol.wsaBuf.buf = ol.pBuf;
	ol.wsaBuf.len = size;
	ol.nOpCode = OPCODE_WRITE;
	//ol.ol.hEvent = (HANDLE)FD_WRITE;
	int nResult = WSASend(m_Socket,&(ol.wsaBuf),1,&(ol.dwSendBytes),ol.dwFlags,&(ol.ol),NULL);
	return;
	if( SOCKET_ERROR == nResult)
	{
		int nErrorCode = GetErrorCode();
		if(nErrorCode != WSA_IO_PENDING && nErrorCode != 0 && nErrorCode != WSAEWOULDBLOCK)
		{
			onError(nErrorCode);
		}
	}
}

VOID ISocketClient::SyncRecv( size_t size )
{
	ZeroMemory(&ol,sizeof(ol));
	ol.plPoint = this;
	ol.wsaBuf.buf = ol.pBuf;
	ol.wsaBuf.len = size;
	ol.nOpCode = OPCODE_READ;
	//ol.ol.hEvent = (HANDLE)FD_READ;
	ol.dwRecvBytes = 0;
	int nResult = WSARecv(m_Socket,&(ol.wsaBuf),1,&(ol.dwRecvBytes),&ol.dwFlags,&(ol.ol),NULL);
	return;
	if( SOCKET_ERROR == nResult)
	{
		int nErrorCode = GetErrorCode();
		if(nErrorCode != WSA_IO_PENDING && nErrorCode != 0 && nErrorCode != WSAEWOULDBLOCK)
		{
			onError(nErrorCode);
		}
	}
}
void ISocketClient::InitMonitThread()
{
	if(!m_hMonitConnectThread)
		m_hMonitConnectThread = (HANDLE)_beginthreadex(0,0,(unsigned int (__stdcall *)(void *))&MonitConnectClient,this,0,0);
}

BOOL ISocketClient::RegToIOCP(ISocket *pContronSocket/* = NULL*/)
{
	if(!m_pIocpPtr)
	return false;
	if(pContronSocket == NULL)
		pContronSocket = this;
	 if(!m_pIocpPtr->BindIoCompletionPort((HANDLE)GetSocket(),(ULONG_PTR)pContronSocket))
		 return FALSE;
	if(GetAlwaysRecv()) 
		SyncRecv(1024);
	return TRUE;
}


DWORD WINAPI ISocketClient::MonitConnectClient( LPVOID pParam )
{
	ISocket *pSocketBase = (ISocket *)pParam;
	theConsole.Trace("ISocket","ClientConnectMonit");
	int nErrorCode = 0;
	int len = sizeof(nErrorCode);
	timeval tm;
	fd_set fdsWrite,fsdRead;
	ISocketClient *pClientSocketPtr = NULL;
	SOCKET Socket = INVALID_SOCKET;
	while (TRUE)
	{
		ClientIte ite = ISocketClient::m_sVecClient.begin();
		for ( ; ite != ISocketClient::m_sVecClient.end(); ++ ite)
		{
			pClientSocketPtr = (*ite);
			Socket = pClientSocketPtr->GetSocket();
			tm.tv_sec  = 0;tm.tv_usec = 20;
			FD_ZERO(&fdsWrite);			FD_ZERO(&fsdRead);
			FD_SET(Socket, &fdsWrite);	FD_SET(Socket, &fsdRead);
			if( select(Socket+1, &fsdRead, &fdsWrite, NULL, &tm) > 0)
			{
				nErrorCode = pClientSocketPtr->GetErrorCode();
				if(nErrorCode == 0) 
				{
					pClientSocketPtr->SetStatus(SOCKET_STATUS_CONNEECTED);
					pClientSocketPtr->RegToIOCP();
					pClientSocketPtr->OnConnect(Socket);
					
				}else
					pClientSocketPtr->onError(nErrorCode);
				//Lock lock(ISocketClient::_mutex);
				ISocketClient::m_sVecClient.erase(ite);
				break;
			}
			else
			{
				nErrorCode = pClientSocketPtr->GetErrorCode();
				//getsockopt(Socket, SOL_SOCKET, SO_ERROR, (char*)&nErrorCode, (int *)&len);
				if( nErrorCode != 0 ) 
				{
					//Lock lock(ISocketClient::_mutex);
					ISocketClient::m_sVecClient.erase(ite);
					pClientSocketPtr->onError(nErrorCode);
					break;
				}
			}
		}
		//Sleep(50);
	}
	_endthreadex(0);
	return 0;
}


unsigned int WINAPI ISocketClient::WorkerThread( LPVOID CompletionPortID )
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
		ISocket *pMainSocket = (ISocket *)re;
		OVERLAPPEDPLUS* pOlp = (OVERLAPPEDPLUS*) pOl;
		ISocketClient *pISocket = (ISocketClient *)pOlp->plPoint;
		if(!bRet)
		{
			nErrorCode = WSAGetLastError();
			if (WSA_WAIT_TIMEOUT != WSAGetLastError())
			{
				pISocket->onError(nErrorCode);
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
				pISocket->onError(nErrorCode);
			else
			{
				pISocket->Close();
			}
			continue;
		}

		theConsole.Debug("WorkerThread","OperaCode:%d SendBytes:%d RecvBytes:%d",pOlp->nOpCode,pOlp->dwSendBytes,pOlp->dwRecvBytes);
		switch(pOlp->nOpCode)
		{
		case OPCODE_READ:
			pOlp->wsaBuf.buf[berByte] = '\0';
			pISocket->OnRecv(pOlp->wsaBuf.buf,berByte);
			break;
		case OPCODE_WRITE:
			pISocket->OnSend(pOlp->wsaBuf.buf,berByte);
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



void ISocketClient::AddToMoniter()
{
	//Lock lock(_mutex);
	m_sVecClient.push_back(this);
	if(!m_hMonitConnectThread)
		InitMonitThread();
}

void ISocketClient::onError( DWORD nErrorCode )
{
	theConsole.Trace("ISocketClient","onError ErrorCode:%d,desc:%s",nErrorCode,strerror(nErrorCode));
	Close();
	
}

void ISocketClient::Reconnect()
{
	theConsole.Trace("ISocketClient","Reconnect");
	SyncConnect(m_pszServerAddr,m_sServerPort);
}


