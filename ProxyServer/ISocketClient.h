#pragma once
#include "ISocket.h"
#include "Mutex.h"

class ISocketClient;
class IOCP;
typedef std::list<ISocketClient*>			ClientList;
typedef std::list<ISocketClient*>::iterator	ClientIte;
class ISocketClient:
	public ISocket
{
public:
	ISocketClient(SOCKET socket);
	ISocketClient(void);
	virtual						~ISocketClient(void);

	void						SyncConnect(const TCHAR *pszRemoteAddress,short sPort);
	void						Reconnect();
	

protected:
	virtual	void				onError(DWORD nErrorCode);
	virtual void				OnConnect(int nConnectID){};
	virtual void				OnClose(){};
	virtual void				OnSend(const TCHAR *pszBuff,size_t size){};
	virtual void				OnRecv(const TCHAR *pszBuff,size_t size){};

private:
	static ClientList			m_sVecClient;
	virtual	void				InitMonitThread();
	void						AddToMoniter();
	static	DWORD WINAPI		MonitConnectClient(LPVOID pParam);
	
	static	Mutex _mutex;

protected:
	static	HANDLE				m_hMonitConnectThread;
	
private:
	static	DWORD	m_dwClientNums;
};

