#ifndef __COMMANDBUFFER_H__
#define __COMMANDBUFFER_H__

#include "Command.h"


WHITEBOX_BEGIN

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


// only one thread to push and one to execute
class CThreadedCommandBuffer
{
public:
	CThreadedCommandBuffer( size_t capacity = 4096u )
		: m_capacity(capacity)
		, m_mask(capacity-1)
		, m_bottom(0)
		, m_top(0)
	{
		m_commands = new char[ capacity ];
	}

	~CThreadedCommandBuffer()
	{
		Clear();
		delete[] m_commands;
	}


	template< class TCommandObj >
	void PushCommand( const TCommandObj& commandObj )
	{
		void*	commandObjectAddr;
		CCommand& command = CCommand::AddCommandToBuffer< TCommandObj >( m_commands + (m_bottom & m_mask), commandObjectAddr );

		new (commandObjectAddr) TCommandObj{ commandObj };

		m_bottom += command.GetTotalSize();
	}

	void	ExecuteCommands()
	{
		size_t cursor = m_top;
		size_t bottom = m_bottom;

		while ( cursor < bottom )
		{
			CCommand* pCommand = (CCommand*)(m_commands + (cursor & m_mask));
			pCommand->ExecuteCommand();
			pCommand->~CCommand();
			cursor += pCommand->GetTotalSize();
		}

		m_top = cursor;
	}

	void Clear()
	{
		size_t cursor = m_top;
		size_t bottom = m_bottom;

		while ( cursor < bottom )
		{
			CCommand* pCommand = (CCommand*)(m_commands + (cursor & m_mask));
			pCommand->~CCommand();
			cursor += pCommand->GetTotalSize();
		}

		m_top = cursor;
	}

private:
	size_t				m_capacity;
	size_t				m_mask;
	volatile size_t		m_bottom;
	volatile size_t		m_top;

	char*				m_commands;
};


WHITEBOX_END

#endif
