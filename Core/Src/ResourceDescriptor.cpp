#include "ResourceDescriptor.h"

#include "ResourceManager.h"

#include <stdio.h>

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

CResourceDescriptor::CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
	: m_name(name)
	, m_path(path)
	, m_extension(extension)
	, m_size(size)
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
	if ( m_refCount == 0 )
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
		printf( "Destroying resource %s\n", m_name.c_str() );
		delete m_pResource;
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
