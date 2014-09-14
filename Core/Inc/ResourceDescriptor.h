#ifndef __RESOURCEDESCRIPTOR_H__
#define __RESOURCEDESCRIPTOR_H__

#include "BaseTypes.h"
#include "Resource.h"


WHITEBOX_BEGIN

class CResourceManager;
class CResourceType;
class CResourceDescriptor
{
public:
	CResourceDescriptor();
	CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager );
	~CResourceDescriptor();

	void				Acquire();
	void				Release();
	
	const String&		GetName() const	{ return m_name; };
	const String&		GetPath() const;
	const String&		GetExtension() const;
	size_t				GetSize() const;

	IResource*			GetResource();
	const IResource*	GetResource() const;
	void				SetResource( IResource* pResource );
	
	CResourceType*		GetResourceType();

private:
	String					m_name;
	String					m_path;
	String					m_extension;
	size_t					m_size;

	IResource*				m_pResource;
	int						m_refCount;
	
	CResourceType*			m_pResourceType;
	CResourceManager*		m_pResourceManager;
};

WHITEBOX_END

#endif