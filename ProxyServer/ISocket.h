#pragma once
#include <list>
#include <process.h>
enum eOVERLAPPED_OPCODES
{
	OPCODE_STOP,
	OPCODE_READ,
	OPCODE_WRITE,	
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
public:
	ISocket(SOCKET socket);
	ISocket(void);
	~ISocket(void);
	SOCKET			GetSocket(){ return m_Socket;};


			int		GetErrorCode();
			void	Close();
			bool	BindToIOCP();

	virtual	VOID	SyncSend(const CHAR * ptszMsg,size_t size);
	virtual	VOID	SyncRecv(size_t size);

	virtual	void	onError(DWORD nErrorCode){};
	virtual void	OnClose(){};
	virtual void	OnSend(const TCHAR *pszBuff,size_t size){};
	virtual void	OnRecv(const TCHAR *pszBuff,size_t size){};

private:
			void	InitSocket();
	static unsigned int	WINAPI WorkerThread(LPVOID CompletionPortID);
private:
	static	int		m_nSocketNums;
	static IOCP		*m_pIocpPtr;
protected:
	char			m_pszServerAddr[256];
	short			m_sServerPort;
	OVERLAPPEDPLUS	ol;
	SOCKET			m_Socket;
};

