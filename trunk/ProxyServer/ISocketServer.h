#pragma once
#include "isocket.h"
#include "IOCP.h"
#include <hash_map>
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

	virtual void		OnRecv(ISocket *pISocketPtr,const TCHAR *pszBuff,size_t size);
	virtual void		OnSend(ISocket *pISocketPtr,const TCHAR *pszBuff,size_t size);
	virtual void		OnError(ISocket *pISocketPtr,DWORD nErrorCode);
	virtual void		OnClose(ISocket *pISocketPtr);
	virtual void		OnClose()  {};

private:
	BOOL				RegToIOCP(ISocket *pClientSocket);

	static DWORD WINAPI	AcceptThread(LPVOID pParam);			//侦听主线程 
	static UINT WINAPI	WorkerThread(LPVOID CompletionPortID);	//主工作线程

	std::hash_map<SOCKET,ISocket*>	m_hashClient;
private:
	static	DWORD				m_dwServerNums;
	OVERLAPPEDPLUS				ol;
	static IOCP					*m_pIocpPtr;

};

