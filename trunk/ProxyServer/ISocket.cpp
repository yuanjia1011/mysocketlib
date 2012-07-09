#include "StdAfx.h"
#include "ISocket.h"
#include <process.h>
#include "IOCP.h"
int		ISocket::m_nSocketNums = 0;

ISocket::ISocket(void):m_Socket(INVALID_SOCKET)
{
	WSAData  wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	InitSocket();
	++ m_nSocketNums;

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
		//if(m_pIocpPtr)
		//{
		//	ol.plPoint = this;
		//	ol.dwFlags = 0;
		//	ol.wsaBuf.buf = NULL;
		//	ol.wsaBuf.len = 0;
		//	ol.nOpCode = 0;
		//	m_pIocpPtr->StopWork(&ol.ol);
		//	delete m_pIocpPtr;m_pIocpPtr = NULL;
		//}
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

INT ISocket::GetErrorCode()
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




eSOCKET_STATUS ISocket::SetStatus( eSOCKET_STATUS eStatus )
{
	eSOCKET_STATUS eOldStatus = m_eStatus ;
	m_eStatus = eStatus;
	OnStatusChanged(eStatus);
	return eOldStatus;
}



INT ISocket::Recv( size_t size )
{
	ZeroMemory(&ol,sizeof(ol));
	ol.plPoint = this;
	ol.wsaBuf.buf = ol.pBuf;
	ol.wsaBuf.len = size;
	ol.nOpCode = OPCODE_READ;
	//ol.ol.hEvent = (HANDLE)FD_READ;
	ol.dwRecvBytes = 0;
	int nResult = WSARecv(m_Socket,&(ol.wsaBuf),1,&(ol.dwRecvBytes),&ol.dwFlags,&(ol.ol),NULL);
	
	//if( SOCKET_ERROR == nResult)
	//{
	//	int nErrorCode = GetErrorCode();
	//	if(nErrorCode != WSA_IO_PENDING && nErrorCode != 0 && nErrorCode != WSAEWOULDBLOCK)
	//	{
	//		onError(nErrorCode);
	//	}
	//}
	return nResult;
}
INT ISocket::Send(const CHAR * ptszMsg,size_t size )
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
	
	//if( SOCKET_ERROR == nResult)
	//{
	//	int nErrorCode = GetErrorCode();
	//	if(nErrorCode != WSA_IO_PENDING && nErrorCode != 0 && nErrorCode != WSAEWOULDBLOCK)
	//	{
	//		onError(nErrorCode);
	//	}
	//}
	return nResult;
}