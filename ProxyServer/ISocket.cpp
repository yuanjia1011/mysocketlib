#include "StdAfx.h"
#include "ISocket.h"
#include <process.h>
#include "IOCP.h"
int		ISocket::m_nSocketNums = 0;
IOCP *	ISocket::m_pIocpPtr = NULL;

ISocket::ISocket(void):m_Socket(INVALID_SOCKET)
{
	WSAData  wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	InitSocket();
	++ m_nSocketNums;
	if(!m_pIocpPtr)
	{
		m_pIocpPtr = new IOCP;
		m_pIocpPtr->Init();
		m_pIocpPtr->StartWork(&WorkerThread);
	}
}

ISocket::ISocket( SOCKET socket )
{
	m_Socket = socket;
}


ISocket::~ISocket(void)
{
	-- m_nSocketNums;
	if( 0 == m_nSocketNums)
	{
		WSACleanup();
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
BOOL ISocket::RegToIOCP()
{
	if(!m_pIocpPtr)
		return false;
	return m_pIocpPtr->BindIoCompletionPort((HANDLE)GetSocket(),(ULONG_PTR)this);
}
VOID ISocket::SyncSend(const CHAR * ptszMsg,size_t size )
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

VOID ISocket::SyncRecv( size_t size )
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

void ISocket::Close()
{
	SetStatus(SOCKET_STATUS_CLOSE);
	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	InitSocket();
	OnClose();
}

int ISocket::GetErrorCode()
{
	int nErrorCode = 0;
	int len = sizeof(nErrorCode);
	getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&nErrorCode, (int *)&len);
	return nErrorCode;
}

void ISocket::InitSocket()
{
	m_Socket =  WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}


unsigned int WINAPI ISocket::WorkerThread( LPVOID CompletionPortID )
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
		OVERLAPPEDPLUS* pOlp = (OVERLAPPEDPLUS*) pOl;
		ISocket *pISocket = (ISocket *)pOlp->plPoint;
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
	}
	_endthreadex(0);
	return TRUE;
}

VOID ISocket::BindIOCP( IOCP * pIocp )
{
	m_pIocpPtr = pIocp;
}

IOCP	* ISocket::GetIOCP()
{
	return m_pIocpPtr;
}

eSOCKET_STATUS ISocket::SetStatus( eSOCKET_STATUS eStatus )
{
	eSOCKET_STATUS eOldStatus = m_eStatus ;
	m_eStatus = eStatus;
	OnStatusChanged(eStatus);
	return eOldStatus;
}
