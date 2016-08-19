#ifndef __THREADDOUBLEBUFFER_H__
#define __THREADDOUBLEBUFFER_H__

#include "CommandBuffer.h"
#include "System/Thread.h"
#include "Command.h"

WHITEBOX_BEGIN

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
		m_sendBufferIdx = 1 - m_sendBufferIdx;
		m_commandMutex.Unlock();
	}

	// Return buffer of commands that will be sent to this worker thread
	CCommandBuffer&	LockSendCommandBuffer()
	{
		m_commandMutex.Lock();
		return m_commandBuffers[ m_sendBufferIdx ];
	}

	void	UnlockSendCommandBuffer()
	{
		m_commandMutex.Unlock();
	}

	// Must be called from this worker thread only (safe, so no lock, since only this thread can swap)
	CCommandBuffer&		GetReceivedCommands()
	{
		return m_commandBuffers[ 1 - m_sendBufferIdx ];
	}

private:
	CMutex				m_commandMutex;
	CCommandBuffer		m_commandBuffers[2];
	size_t				m_sendBufferIdx;
};

WHITEBOX_END

#endif
