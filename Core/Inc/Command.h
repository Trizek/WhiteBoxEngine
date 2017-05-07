#ifndef __COMMAND_H__
#define __COMMAND_H__


WHITEBOX_BEGIN

class CCommand
{
public:
	typedef void(*TCommandFunc)(void*);

	template< class TCommandObj >
	static CCommand& AddCommandToBuffer( void* pBuffer, const TCommandObj& commandObject )
	{
		CCommand* pCommand = new (pBuffer) CCommand{ &Execute<TCommandObj>, &Destroy<TCommandObj> };
		new (pCommand->m_pCommandObject) TCommandObj{ commandObject };
		return *pCommand;
	}

	CCommand( TCommandFunc executeFunc, TCommandFunc destroyFunc )
		: m_executeFunc(executeFunc)
		, m_destroyFunc(destroyFunc)
	{}

	~CCommand()
	{
		m_destroyFunc(m_pCommandObject);
	}

	void	ExecuteCommand()
	{
		m_executeFunc(m_pCommandObject);
	}

private:
	template< class Type >
	void SetCommandObject( void* commandObjectAddr )
	{
		m_pCommandObject = commandObjectAddr;
		m_totalSize = sizeof(CCommand) + sizeof(Type); // padding ?

	}

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

	static const size_t s_MaxCommandObjectSize = 512;

	TCommandFunc	m_executeFunc;
	TCommandFunc	m_destroyFunc;
	char			m_pCommandObject[ s_MaxCommandObjectSize ];
};

WHITEBOX_END

#endif
