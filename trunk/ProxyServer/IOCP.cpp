#include "StdAfx.h"
#include "IOCP.h"

#include "ISocket.h"
IOCP::IOCP(void)
{
}


IOCP::~IOCP(void)
{
}

void IOCP::Init(int nThreadNums )
{
	if(0 == nThreadNums)
	{
		SYSTEM_INFO SystemInfo;
		UINT i = 0;
		unsigned int dwThreadID = 0;
		GetSystemInfo(&SystemInfo);
		nThreadNums = SystemInfo.dwNumberOfProcessors * 2 + 1;
	}
	m_nWorkThreadsNum = nThreadNums;
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_nWorkThreadsNum);
	ZeroMemory(m_hThreadHandles,sizeof(m_hThreadHandles));
}

VOID IOCP::Bind( HANDLE hFileHandle,ULONG_PTR CompletionKey,DWORD NumberOfConcurrentThreads /*= 0 */ )
{
	CreateIoCompletionPort(hFileHandle, m_hCompletionPort, CompletionKey, NumberOfConcurrentThreads);
}

VOID IOCP::StartWork(pWorkThreadFun pWorkFun)
{
	if(!pWorkFun)
		return;
	unsigned int dwThreadID = 0;
	for(int i=0; i< m_nWorkThreadsNum ;++i)
	{
		m_hThreadHandles[i] = (HANDLE)_beginthreadex(NULL, 0, pWorkFun, m_hCompletionPort, 0, &dwThreadID);
		if(m_hThreadHandles[i] == NULL)
		{
			printf("CreateThread() failed with error %d\n", GetLastError());
			return ;
		}
		CloseHandle(m_hThreadHandles[i]);
	}

}

VOID IOCP::StopWork(LPOVERLAPPED lpOverlapped)
{
	for (int i= 0 ; i < m_nWorkThreadsNum; ++i)
	{
		PostQueuedCompletionStatus(m_hCompletionPort,0,NULL,lpOverlapped);
	}
}
