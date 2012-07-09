#pragma once
#include "isocket.h"
class CFormClientSocket :
	public ISocket
{
public:
	CFormClientSocket(SOCKET socket);
	CFormClientSocket(void);
	virtual ~CFormClientSocket(void);

protected:
	//virtual	void	onError(DWORD nErrorCode) ;
	virtual void	OnClose() ;
	virtual void	OnSend(const TCHAR *pszBuff,size_t size) ;
	virtual void	OnRecv(const TCHAR *pszBuff,size_t size) ;
};

