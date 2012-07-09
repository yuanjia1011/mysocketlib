#pragma once
#include "isocketserver.h"
class CServer :
	public ISocketServer
{
public:
	CServer(void);
	~CServer(void);
	virtual	void	OnStartListen();
	virtual	void	OnAccept(ISocket *pISocketPtr);

	virtual	ISocket*	AllocClientSocket(SOCKET ClientSocket);
};

