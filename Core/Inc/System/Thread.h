#ifndef __THREAD_H__
#define __THREAD_H__

#include "BaseTypes.h"

#include <thread>
#include <mutex>

WHITEBOX_BEGIN

class CMemberCallback
{
public:
	typedef void(*TFunctionPointer)(void*);

	CMemberCallback()
		: m_pObject(nullptr)
		, m_pStaticFunction(nullptr){}

	template< class Type, void (Type::*MemberFunc)(void) >
	CMemberCallback& Bind(Type& type)
	{
		m_pObject = &type;
		m_pStaticFunction = &StaticCallback< Type, MemberFunc >;

		return *this;
	}

	void operator()() const
	{
		if ( m_pObject != nullptr && m_pStaticFunction != nullptr )
		{
			m_pStaticFunction( m_pObject );
		}
	}

	TFunctionPointer	GetCallback() const
	{
		return m_pStaticFunction;
	}

	void*				GetObject() const
	{
		return m_pObject;
	}

private:

	template< class Type, void (Type::*MemberFunc)(void) >
	static void		StaticCallback( void* pObject )
	{
		Type& object = *((Type*)pObject);
		(object.*MemberFunc)();
	}

	void*				m_pObject;
	TFunctionPointer	m_pStaticFunction;
};

class CThread
{
public:
	CThread( const CMemberCallback& callback );

	void	WaitForFinish();

private:
	std::thread		m_thread;
};

class CMutex
{
public:
	void			Lock();
	bool			TryLock();
	void			Unlock();

private:
	std::mutex		m_mutex;
};


class CWorkerThread
{
public:
	CWorkerThread()
		: m_pThread(nullptr)
	{}

	void	Run( const CMemberCallback& callback )
	{
		m_pThread = new CThread( callback );
	}

	void	WaitForFinish()
	{
		m_pThread->WaitForFinish();
	}

private:
	CThread*			m_pThread;
};

WHITEBOX_END

#endif
