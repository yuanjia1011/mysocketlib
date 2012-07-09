#pragma once
class Mutex
{
public:
	Mutex(void);
	~Mutex(void);
	void Lock();
	void UnLock();
private:
	CRITICAL_SECTION m_criticalSection;
};

class Lock
{
public:
	Lock(Mutex &mutex);
	~Lock(void);
	void SetUnlock();
	operator bool() const;
private:
	Mutex &	m_mutex;
	bool	m_locked;
};

