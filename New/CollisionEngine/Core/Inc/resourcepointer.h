#ifndef __RESOURCEPOINTER_H__
#define __RESOURCEPOINTER_H__

#include "BaseTypes.h"
#include "ResourceDescriptor.h"


WHITEBOX_BEGIN

template< class TResourceType >
class CResourcePointer
{
public:
	CResourcePointer()
		: m_pDescriptor(NULL){}
<<<<<<< HEAD


	CResourcePointer( TResourceType* pResource )
	{
		m_pDescriptor = new CResourceDescriptor( "", "", "", 0, true, nullptr, nullptr );
		m_pDescriptor->SetResource( pResource );

		if ( m_pDescriptor )
		{
			m_pDescriptor->Acquire();
		}
	}

=======
		
>>>>>>> master
	CResourcePointer( CResourceDescriptor* pDescriptor )
		: m_pDescriptor(pDescriptor)
	{
		if ( m_pDescriptor )
		{
			m_pDescriptor->Acquire();
		}	
	}		
								
	CResourcePointer( const CResourcePointer< TResourceType >& pResource )
		: m_pDescriptor(pResource.m_pDescriptor)
	{
		if ( m_pDescriptor )
		{
			m_pDescriptor->Acquire();
		}
	}

	~CResourcePointer()
	{
		if ( m_pDescriptor )
		{
			m_pDescriptor->Release();
		}
	}
	
	CResourcePointer& operator=( const CResourcePointer< TResourceType >& pResource )
	{
		if ( m_pDescriptor )
		{
			m_pDescriptor->Release();
		}
		m_pDescriptor = pResource.m_pDescriptor;
		if ( m_pDescriptor )
		{
			m_pDescriptor->Acquire();
		}

		return *this;
	}	

	const TResourceType*	operator->() const
	{
		return static_cast< TResourceType* >( m_pDescriptor->GetResource() );
	}
	
	TResourceType*			operator->()
	{
		return static_cast< TResourceType* >( m_pDescriptor->GetResource() );
	}
	
	const TResourceType*	get() const
	{
		if ( m_pDescriptor )
		{
<<<<<<< HEAD
			return static_cast< const TResourceType* >( m_pDescriptor->GetResource() );
=======
			return m_pDescriptor->GetResource();
>>>>>>> master
		}
		else
		{
			return NULL;
		}
	}
	
	TResourceType*			get()
	{
		if ( m_pDescriptor )
		{
			return static_cast<TResourceType*>( m_pDescriptor->GetResource() );
		}
		else
		{
			return NULL;
		}
	}
	
	void reset()
	{
		if ( m_pDescriptor )
		{
			m_pDescriptor->Release();
			m_pDescriptor = NULL;
		}
	}
	
	operator bool() const
	{
		return ( m_pDescriptor != NULL && m_pDescriptor->GetResource() != NULL );
	}	
		
private:
	CResourceDescriptor*	m_pDescriptor;
};

#define DECLARE_RESOURCE_POINTER( name ) \
	typedef CResourcePointer< name > name##Ptr; \


WHITEBOX_END

#endif