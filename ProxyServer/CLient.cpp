#include "StdAfx.h"
#include "CLient.h"


CCLient::CCLient(void)
{
}


CCLient::~CCLient(void)
{
}

void CCLient::OnConnect( int nConnectID )
{
	theConsole.Trace("Client","OnConnected ConnectID:%d",nConnectID);
	std::string strHttpReq = "";								  
	strHttpReq += "GET / HTTP/1.1\r\n";
	strHttpReq += "Accept: text/html, application/xhtml+xml, */*\r\n";
	strHttpReq += "Accept-Language: zh-CN\r\n";
	strHttpReq += "User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n";
	strHttpReq += "Accept-Encoding: gzip, deflate\r\n";
	strHttpReq += "Host: 127.0.0.1:80\r\n";
	strHttpReq += "Connection: Keep-Alive\r\n";
	SyncSend(strHttpReq.c_str(),strHttpReq.length()+1);
}

void CCLient::OnClose()
{
	theConsole.Trace("OnClose","");
	Reconnect();
}

void CCLient::OnSend( const TCHAR *pszBuff,size_t size )
{
	theConsole.Trace("OnSend","\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n%s\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",pszBuff);
	SyncRecv(1024);
}

void CCLient::OnRecv( const TCHAR *pszBuff,size_t size )
{
	theConsole.Trace("OnRecv","\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n%s\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",pszBuff);
	SyncRecv(1024);
}
