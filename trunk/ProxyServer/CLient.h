#pragma once
#include "ISocketClient.h"
class CCLient:public ISocketClient
{
public:
	CCLient(void);
	virtual ~CCLient(void);

	virtual void	OnConnect(int nConnectID);
	virtual void	OnClose();
	virtual void	OnSend(const TCHAR *pszBuff,size_t size);
	virtual void	OnRecv(const TCHAR *pszBuff,size_t size);
};

