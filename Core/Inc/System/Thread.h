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


class CCommand
{
public:
	template< class Type >
	static CCommand& AddCommandToBuffer( void* bufferData, void*& commandObjectAddr )
	{
		CCommand* pCommand = new (bufferData) CCommand();
		commandObjectAddr = ((char*)bufferData) + sizeof(CCommand);
		pCommand->SetCommandObject< Type >( commandObjectAddr );

		return *pCommand;
	}

	~CCommand()
	{
		m_destroyFunc(m_pCommandObject);
	}

	void	ExecuteCommand()
	{
		m_executeFunc(m_pCommandObject);
	}

	size_t	GetTotalSize() const
	{
		return m_totalSize;
	}

private:
	template< class Type >
	void SetCommandObject( void* commandObjectAddr )
	{
		m_pCommandObject = commandObjectAddr;
		m_totalSize = sizeof(CCommand) + sizeof(Type); // padding ?
		m_executeFunc = &Execute< Type >;
		m_destroyFunc = &Destroy< Type >;
	}

	typedef void(*TCommandFunc)(void*);

	template< class Type >
	static void Execute( void* pCommandObject )
	{
		((Type*)pCommandObject)->Execute();
	}

	template< class Type >
	static void Destroy( void* pCommandObject )
	{
		((Type*)pCommandObject)->~Type();
	}

	void*			m_pCommandObject;
	size_t			m_totalSize;
	TCommandFunc	m_executeFunc;
	TCommandFunc	m_destroyFunc;
};


class CCommandBuffer
{
public:
	CCommandBuffer( size_t maxSize = 4096 )
		: m_maxSize(maxSize)
		, m_size(0)
	{
		m_commands = new char[ m_maxSize ];
	}

	~CCommandBuffer()
	{
		Clear();
		delete[] (char*)m_commands;
	}

	template< class Type >
	void* AddCommandToBuffer()
	{
		void*	commandObjectAddr;
		CCommand& command = CCommand::AddCommandToBuffer< Type >( ((char*)m_commands) + m_size, commandObjectAddr );
		m_size += command.GetTotalSize();

		return commandObjectAddr;
	}

	void	ExecuteCommands()
	{
		size_t cursor = 0;
		while (cursor < m_size)
		{
			CCommand* pCommand = (CCommand*)((char*)m_commands + cursor);
			pCommand->ExecuteCommand();
			cursor += pCommand->GetTotalSize();
		}
	}

	void	Clear()
	{
		size_t cursor = 0;
		while (cursor < m_size)
		{
			CCommand* pCommand = (CCommand*)((char*)m_commands + cursor);
			pCommand->~CCommand();
			cursor += pCommand->GetTotalSize();
		}
		m_size = 0;
	}


private:
	void*		m_commands;
	size_t		m_maxSize;
	size_t		m_size;
};


// Commands sent from one thread to another (unidirectional)
class CThreadDoubleBuffer
{
public:
	CThreadDoubleBuffer()
		: m_sendBufferIdx(0){}

	// Must be called from this worker thread only
	void	SwapBuffers()
	{
		m_commandMutex.Lock();
		printf("Swap buffers...\n");
		m_sendBufferIdx = 1 - m_sendBufferIdx;
		m_commandMutex.Unlock();
	}

	// Return buffer of commands that will be sent to this worker thread
	CCommandBuffer&	LockSendCommandBuffer()
	{
		m_commandMutex.Lock();
		return m_commandBuffers[m_sendBufferIdx];
	}

	void	UnlockSendCommandBuffer()
	{
		m_commandMutex.Unlock();
	}

	// Must be called from this worker thread only (safe, so no lock, since only this thread can swap)
	CCommandBuffer&		GetReceivedCommands()
	{
		return m_commandBuffers[1 - m_sendBufferIdx];
	}

private:
	CMutex				m_commandMutex;
	CCommandBuffer		m_commandBuffers[2];
	size_t				m_sendBufferIdx;
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
