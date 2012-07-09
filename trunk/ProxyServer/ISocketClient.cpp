#include "StdAfx.h"
#include "ISocketClient.h"
#include "IOCP.h"
ClientList	ISocketClient::m_sVecClient;
HANDLE		ISocketClient::m_hMonitConnectThread = NULL;
DWORD		ISocketClient::m_dwClientNums = 0;
Mutex		ISocketClient::_mutex;
ISocketClient::ISocketClient(void)
{
	m_dwClientNums ++;

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

void ISocketClient::InitMonitThread()
{
	if(!m_hMonitConnectThread)
		m_hMonitConnectThread = (HANDLE)_beginthreadex(0,0,(unsigned int (__stdcall *)(void *))&MonitConnectClient,this,0,0);
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
					pClientSocketPtr->BindToIOCP();
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

