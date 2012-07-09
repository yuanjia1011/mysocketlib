#pragma once
#include <list>
#include <process.h>
enum eOVERLAPPED_OPCODES
{
	OPCODE_STOP,
	OPCODE_READ,
	OPCODE_WRITE,	
};
enum eSOCKET_STATUS
{
	SOCKET_INVALID,
	SOCKET_STATUS_INITED,
	SOCKET_STATUS_CONNECTING,
	SOCKET_STATUS_CONNEECTED,
	SOCKET_STATUS_LISTENING,
	SOCKET_STATUS_READY_FOR_READ,
	SOCKET_STATUS_READY_FOR_WRITE,
	SOCKET_STATUS_CLOSE,
	SOCKET_MAX
};
typedef struct _OVERLAPPEDPLUS
{
	OVERLAPPED	ol;
	LPVOID		plPoint;
	int			nOpCode;
	WSABUF		wsaBuf;
	DWORD		dwFlags;
	DWORD		dwSendBytes;
	DWORD		dwRecvBytes;
	char		pBuf[4096];
}OVERLAPPEDPLUS, *POVERLAPPEDPLUS;

class IOCP;
class ISocket
{
	friend	class ISocketClient;
	friend	class ISocketServer;
public:
	ISocket(SOCKET socket);
	ISocket(void);
	~ISocket(void);
	SOCKET			GetSocket(){ return m_Socket;};


			int		GetErrorCode();
			void	Close();
			// 取得当前状态 [7/8/2012 Peter]
	
	eSOCKET_STATUS	GetStatus(){ return m_eStatus;};

			VOID	SyncSend(const CHAR * ptszMsg,size_t size);
			VOID	SyncRecv(size_t size);


protected:

	virtual	void	onError(DWORD nErrorCode) {};
	virtual void	OnClose()  {};
	virtual void	OnSend(const TCHAR *pszBuff,size_t size) {};
	virtual void	OnRecv(const TCHAR *pszBuff,size_t size) {};
	virtual void	OnStatusChanged(eSOCKET_STATUS eStatus) {};
private:
			VOID	BindIOCP(IOCP * pIocp);
			IOCP*	GetIOCP();
			BOOL	RegToIOCP();
			void	InitSocket();
	eSOCKET_STATUS	SetStatus(eSOCKET_STATUS eStatus);		
	static unsigned int	WINAPI WorkerThread(LPVOID CompletionPortID);
private:
	static	int		m_nSocketNums;
	static	IOCP	*m_pIocpPtr;
	eSOCKET_STATUS	m_eStatus;
protected:
	char			m_pszServerAddr[256];
	short			m_sServerPort;
	OVERLAPPEDPLUS	ol;
	SOCKET			m_Socket;
};

