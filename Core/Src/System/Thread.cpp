#include "System/Thread.h"


WHITEBOX_BEGIN


CThread::CThread( const CMemberCallback& callback )
	: m_thread(callback.GetCallback(), callback.GetObject())
{
}

void	CThread::WaitForFinish()
{
	m_thread.join();
}

void	CMutex::Lock()
{
	m_mutex.lock();
}

bool	CMutex::TryLock()
{
	return m_mutex.try_lock();
}

void	CMutex::Unlock()
{
	m_mutex.unlock();
}

WHITEBOX_END