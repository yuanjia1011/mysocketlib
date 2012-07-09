#pragma once
#include "isocket.h"
class IOCP;
class ISocketServer :
	public ISocket
{
public:
	ISocketServer(void);
	virtual ~ISocketServer(void);

			bool		StartListen(short sPort,bool bSharePort = false);
			void		StopListen();
protected:
	virtual	void		OnStartListen(){};
	virtual	void		OnAccept(ISocket *pISocketPtr)			= 0;
	virtual	ISocket*	AllocClientSocket(SOCKET ClientSocket)	= 0;
	virtual void	OnClose()  {};

private:
	static	DWORD WINAPI		AcceptThread(LPVOID pParam);
	static unsigned int	WINAPI	WorkerThread(LPVOID CompletionPortID);
private:
	static IOCP					*m_pIocpPtr;

};

