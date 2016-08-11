#ifndef __SMARTPOINTER_H__
#define __SMARTPOINTER_H__

#define SAFE_DELETE(pointer) if (pointer){ delete pointer; pointer = NULL; }

WHITEBOX_BEGIN

// Intrusive pointer so that refcount cannot be separated from data
// (Would be dramatic to have 2 refcounts for one data, could delete data with valid pointers on it)
struct ISmartPointerData
{
	virtual void	IncreaseRefCount() = 0;
	virtual int		DecreaseRefCount() = 0;
	
	virtual ~ISmartPointerData(){}
};

class CSmartPointerData : public ISmartPointerData
{
public:
	CSmartPointerData()
		: m_refCount(0){}

	virtual void	IncreaseRefCount() override
	{
		++m_refCount;
	}
	
	virtual int		DecreaseRefCount() override
	{
		return --m_refCount;
	}
	
private:
	int	m_refCount;
};

template< class Type >
class CSmartPointer
{
public:
	CSmartPointer()
		: m_pData(NULL){}
		
	CSmartPointer( Type* pointer )
		: m_pData(pointer)
	{
		IncreaseRefCount();
	}
		
	CSmartPointer( const CSmartPointer& smartPtr )
		: m_pData(smartPtr.m_pData)
	{
		IncreaseRefCount();
	}		
		
	~CSmartPointer()
	{
		DecreaseRefCount();
	}
	
	CSmartPointer& operator=( const CSmartPointer& smartPtr )
	{
		DecreaseRefCount();
		m_pData = smartPtr.m_pData;
		IncreaseRefCount();

		return *this;
	}
	
	CSmartPointer& operator=( Type* pointer )
	{
		DecreaseRefCount();
		m_pData = pointer;
		IncreaseRefCount();
		
		return *this;
	}	
	
	const Type* operator->() const
	{
		return m_pData;
	}
	
	Type* operator->()
	{
		return m_pData;
	}
	
	const Type* get() const
	{
		return m_pData;
	}
	
	Type* get()
	{
		return m_pData;
	}
	
	void reset()
	{
		DecreaseRefCount();
		m_pData = NULL;
	}
	
	operator bool() const
	{
		return ( m_pData != NULL );
	}
	

private:
	void	IncreaseRefCount()
	{
		if ( m_pData )
		{
			ISmartPointerData* pBaseData = m_pData; // remove the interface, call directly Increase
			pBaseData->IncreaseRefCount();
		}	
	}
	
	void	DecreaseRefCount()
	{
		if ( m_pData )
		{
			ISmartPointerData* pBaseData = m_pData; // Same here (avoid vtable)
			if ( pBaseData->DecreaseRefCount() == 0 )
			{
				SAFE_DELETE( pBaseData );
			}
		}	
	}

	Type*	m_pData;
};

template< class Type >
class CSmartPointerConst
{
public:
	CSmartPointerConst()
		: m_pData(NULL){}
		
	CSmartPointerConst( const Type* pointer )
		: m_pData(pointer)
	{
		IncreaseRefCount();
	}
	
	CSmartPointerConst( const CSmartPointerConst& smartPtr )
		: m_pData(smartPtr.m_pData)
	{
		IncreaseRefCount();
	}	
		
	~CSmartPointerConst()
	{
		DecreaseRefCount();
	}
	
	CSmartPointerConst& operator=( const CSmartPointerConst& smartPtr )
	{
		DecreaseRefCount();
		m_pData = smartPtr.m_pData;
		IncreaseRefCount();
	}
	
	CSmartPointerConst& operator=( Type* pointer )
	{
		DecreaseRefCount();
		m_pData = pointer;
		IncreaseRefCount();
	}	
	
	const Type* operator->() const
	{
		return m_pData;
	}

	const Type* get() const
	{
		return m_pData;
	}

	operator bool() const
	{
		return ( m_pData != NULL );
	}

private:
	void	IncreaseRefCount()
	{
		if ( m_pData )
		{
			ISmartPointerData* pBaseData = const_cast< Type* >( m_pData );
			pBaseData->IncreaseRefCount();
		}	
	}
	
	void	DecreaseRefCount()
	{
		if ( m_pData )
		{
			ISmartPointerData* pBaseData = const_cast< Type* >( m_pData );
			if ( pBaseData->DecreaseRefCount() == 0 )
			{
				SAFE_DELETE( pBaseData );
			}
		}	
	}

	const Type*	m_pData;
};


#define DEFINE_SMART_PTR(ClassName) typedef CSmartPointer< ClassName > ClassName##Ptr; \
									typedef CSmartPointerConst< ClassName > ClassName##ConstPtr; \

WHITEBOX_END

#endif