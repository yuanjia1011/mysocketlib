#pragma once
class ISocket;

typedef unsigned int (__stdcall *pWorkThreadFun)(void *) ;
#define MAX_WORK_THREADS	10


class IOCP
{
public:
	IOCP(void);
	virtual ~IOCP(void);
	VOID	Init(int nThreadNums = 0);
	BOOL	BindIoCompletionPort(HANDLE hFileHandle,ULONG_PTR CompletionKey,DWORD NumberOfConcurrentThreads = 0 );
	VOID	StartWork(pWorkThreadFun pWorkFun);
	VOID	StopWork(LPOVERLAPPED lpOverlapped);
	

protected:
	HANDLE	m_hCompletionPort;
	HANDLE	m_hThreadHandles[MAX_WORK_THREADS];
	int		m_nWorkThreadsNum;
};

