#ifndef __COMMAND_H__
#define __COMMAND_H__


WHITEBOX_BEGIN

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

WHITEBOX_END

#endif
