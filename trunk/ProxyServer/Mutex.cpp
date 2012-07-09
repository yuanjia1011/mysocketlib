#include "StdAfx.h"
#include "Mutex.h"

#define synchronized(M)		for(Lock M##_lock = M; M##_lock; M##_lock.SetUnlock())//

Mutex::Mutex(void)
{
	InitializeCriticalSection(&m_criticalSection);
}

Mutex::~Mutex(void)
{
	DeleteCriticalSection(&m_criticalSection);//��֤��������ʱ���ܹ�ɾ���ٽ���
}
void Mutex::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}

void Mutex::UnLock()
{
	LeaveCriticalSection(&m_criticalSection);

}

Lock::Lock(Mutex &mutex):m_mutex(mutex),m_locked(true)
{
	m_mutex.Lock();
}

Lock::~Lock(void)
{
	/*һ��Ҫ�����������н�������Ϊ���ܷ���ʲô��ֻҪ�����뿪�����������ڣ����뿪�����ţ��������������������*/
	m_mutex.UnLock();
}
void Lock::SetUnlock()
{
	m_locked = false;
}

Lock::operator bool() const
{
	return m_locked;


}

