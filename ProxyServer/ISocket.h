#pragma once
#include <list>
#include <process.h>
#include "IOCP.h"
enum eOVERLAPPED_OPCODES
{
	OPCODE_STOP,
	OPCODE_READ,
	OPCODE_WRITE,	
};
enum eSOCKET_TYPE
{
	SOCKET_CLIENT,
	SOCKET_SERVER,
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

class ISocket
{
	friend	class ISocketClient;
	friend	class ISocketServer;
public:
	ISocket(SOCKET socket);
	ISocket(void);
	~ISocket(void);
	SOCKET			GetSocket(){ return m_Socket;};

			INT		Send(const CHAR * ptszMsg,size_t size );
			INT		Recv(size_t size );

			INT		GetErrorCode();
			void	Close();
	eSOCKET_STATUS	GetStatus(){ return m_eStatus;};

protected:

	virtual	void	onError(DWORD nErrorCode) {};
	virtual void	OnClose()  {};
	virtual void	OnSend(const TCHAR *pszBuff,size_t size) {};
	virtual void	OnRecv(const TCHAR *pszBuff,size_t size) {};
	virtual void	OnStatusChanged(eSOCKET_STATUS eStatus) {};

private:	
			void	InitSocket();
	eSOCKET_STATUS	SetStatus(eSOCKET_STATUS eStatus);		
private:
	static	int		m_nSocketNums;
	eSOCKET_STATUS	m_eStatus;
	eSOCKET_TYPE	m_eSocketType;
	OVERLAPPEDPLUS	ol;
protected:
	char			m_pszServerAddr[256];
	short			m_sServerPort;
	SOCKET			m_Socket;
};

