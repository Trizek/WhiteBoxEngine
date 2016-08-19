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

WHITEBOX_END

#endif
