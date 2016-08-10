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
<<<<<<< HEAD
	CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, bool bProcedural, CResourceType* pResourceType, CResourceManager* pResourceManager );
=======
	CResourceDescriptor( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager );
>>>>>>> master
	~CResourceDescriptor();

	void				Acquire();
	void				Release();
	void				DestroyIfNoRef();
	
	const String&		GetName() const	{ return m_name; };
	const String&		GetPath() const;
	const String&		GetExtension() const;
	size_t				GetSize() const;
<<<<<<< HEAD
	bool				IsProcedural() const;

=======
>>>>>>> master
	const std::vector< String >&	GetDependencies() const;
	std::vector< String >&			GetDependencies();

	int					GetRefCount() const { return m_refCount; }

	IResource*			GetResource();
	const IResource*	GetResource() const;
	void				SetResource( IResource* pResource );
	
	CResourceType*		GetResourceType();

private:
	String					m_name;
	String					m_path;
	String					m_extension;
	size_t					m_size;
<<<<<<< HEAD
	bool					m_bProcedural;
=======
>>>>>>> master
	std::vector< String >	m_dependencies;

	IResource*				m_pResource;
	int						m_refCount;
	
	CResourceType*			m_pResourceType;
	CResourceManager*		m_pResourceManager;
};

WHITEBOX_END

#endif