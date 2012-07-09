#pragma once
#include "isocket.h"
class IOCP;
class ISocketServer :
	public ISocket
{
public:
	ISocketServer(void);
	virtual ~ISocketServer(void);

			bool	StartListen(short sPort,bool bSharePort = false);
			void	StopListen();

protected:
	virtual	void	OnStartListen(){};
	virtual	void	OnAccept(ISocket *pISocketPtr) = 0;

private:
	static	DWORD WINAPI		AcceptThread(LPVOID pParam);
private:
	static IOCP					*m_pIocpPtr;

};

