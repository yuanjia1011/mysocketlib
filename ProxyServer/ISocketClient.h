#pragma once
#include "ISocket.h"
#include "Mutex.h"
#include "IOCP.h"
class ISocketClient;
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
	VOID						SyncSend(const CHAR * ptszMsg,size_t size );
	VOID						SyncRecv(size_t size );
	void						Reconnect();

	VOID						SetAlwaysRecv(BOOL bAlwaysRecv = TRUE){ m_bAlwaysRecv = bAlwaysRecv;};
	BOOL						GetAlwaysRecv(){ return m_bAlwaysRecv;};
	

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
	static unsigned int	WINAPI	WorkerThread(LPVOID CompletionPortID);
			BOOL				RegToIOCP(ISocket *pContronSocket = NULL);
	
	static	Mutex _mutex;

protected:
	static	HANDLE				m_hMonitConnectThread;
	
private:
	static	DWORD	m_dwClientNums;
	static IOCP					*m_pIocpPtr;
	BOOL						m_bAlwaysRecv;
};

