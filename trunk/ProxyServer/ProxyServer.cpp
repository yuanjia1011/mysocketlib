// ProxyServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include "CLient.h"
#include "Server.h"

int _tmain(int argc, _TCHAR* argv[])
{
	theConsole.Trace("ServerMain","Init");
	CCLient client;
	CServer server;
	//client.SyncConnect("127.0.0.1",1001);
	server.StartListen(99);
	getchar();
	return 0;
}

