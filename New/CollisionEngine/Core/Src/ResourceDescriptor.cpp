#include "ResourceDescriptor.h"

#include "ResourceManager.h"

#include <stdio.h>
#include "GlobalVariables.h"

WHITEBOX_BEGIN

CResourceDescriptor::CResourceDescriptor()
	: m_size(0)
	, m_pResource(NULL)
	, m_refCount(0)
	, m_pResourceType(NULL)
	, m_pResourceManager(NULL)
{
	printf("Create empty descriptor \n");
}

<<<<<<< HEAD
CResourceDescriptor::CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, bool bProcedural, CResourceType* pResourceType, CResourceManager* pResourceManager )
=======
CResourceDescriptor::CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
>>>>>>> master
	: m_name(name)
	, m_path(path)
	, m_extension(extension)
	, m_size(size)
<<<<<<< HEAD
	, m_bProcedural(bProcedural)
=======
>>>>>>> master
	, m_pResource(NULL)
	, m_refCount(0)
	, m_pResourceType(pResourceType)
	, m_pResourceManager(pResourceManager)
{
	printf("Create descriptor %s\n", name.c_str());
}

CResourceDescriptor::~CResourceDescriptor()
{
	if ( m_name.empty())
	printf("Destroying empty descriptor\n");
	else
	printf("Desotrying descriptor %s\n", m_name.c_str());
}

void CResourceDescriptor::Acquire()
{
<<<<<<< HEAD
	if ( m_refCount == 0 && m_pResource == nullptr && !m_bProcedural ) // resource can be not null with refcount 0 (unload then load the same frame)
=======
	if ( m_refCount == 0 && m_pResource == nullptr ) // resource can be not null with refcount 0 (unload then load the same frame)
>>>>>>> master
	{
		// async load
		printf( "Request loading resource %s\n", m_name.c_str() );
		m_pResourceManager->AddLoadResourceQuery( *this );
	}
	m_refCount++;
}

void CResourceDescriptor::Release()
{
	--m_refCount;
	if ( m_refCount == 0 )
	{
<<<<<<< HEAD
		if ( m_bProcedural )
		{
			delete m_pResource;
			delete this;
		}
		else
		{
			printf( "Marking %s to destroy\n", m_name.c_str() );
			m_pResourceManager->AddUnloadResourceQuery( *this );
		}
=======
		printf( "Marking %s to destroy\n", m_name.c_str() );
		m_pResourceManager->AddUnloadResourceQuery( *this );
>>>>>>> master
	}
}

void CResourceDescriptor::DestroyIfNoRef()
{
	if ( m_pResource != nullptr && m_refCount == 0 )
	{
		printf( "Destroy resource %s\n", m_name.c_str());
		delete m_pResource;
		m_pResource = nullptr;
	}
}

const String& CResourceDescriptor::GetPath() const
{
	return m_path;
}

const String& CResourceDescriptor::GetExtension() const
{
	return m_extension;
}

size_t CResourceDescriptor::GetSize() const
{
	return m_size;
}

<<<<<<< HEAD
bool CResourceDescriptor::IsProcedural() const
{
	return m_bProcedural;
}

=======
>>>>>>> master
const std::vector< String >&	CResourceDescriptor::GetDependencies() const
{
	return m_dependencies;
}

std::vector< String >&			CResourceDescriptor::GetDependencies()
{
	return m_dependencies;
}

IResource* CResourceDescriptor::GetResource()
{
	return m_pResource;
}

const IResource* CResourceDescriptor::GetResource() const
{
	return m_pResource;
}

void CResourceDescriptor::SetResource( IResource* pResource )
{
	m_pResource = pResource;
}

CResourceType* CResourceDescriptor::GetResourceType()
{
	return m_pResourceType;
}

WHITEBOX_END
