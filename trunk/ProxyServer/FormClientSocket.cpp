#include "StdAfx.h"
#include "FormClientSocket.h"


CFormClientSocket::CFormClientSocket(void)
{
}

CFormClientSocket::CFormClientSocket( SOCKET socket ):ISocket(socket)
{

}


CFormClientSocket::~CFormClientSocket(void)
{
}

void CFormClientSocket::OnRecv( const TCHAR *pszBuff,size_t size )
{
	theConsole.Trace("OnRecv","\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n%s\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",pszBuff);
	//SyncRecv(1024);
}

void CFormClientSocket::OnSend( const TCHAR *pszBuff,size_t size )
{
	theConsole.Trace("OnSend","\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n%s\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",pszBuff);
	//SyncRecv(1024);
}

void CFormClientSocket::OnClose()
{
	theConsole.Trace("OnClose","客户端主动断开连接");
}
