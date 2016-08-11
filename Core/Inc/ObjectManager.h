#ifndef __OBJECTMANAGER_H__
#define __OBJECTMANAGER_H__

#include "Object.h"

#include "MeshRenderComponent.h"

WHITEBOX_BEGIN

class CObjectManager
{
public:
	CObjectManager();
	
	CObjectPtr	GetObject( Handle handle );		
	CObjectPtr	CreateObject( const String& name, Handle forceHandle = 0 );	
	void		RemoveObject( Handle handle );
	
	CSerializableFactory< IComponent >&	GetComponentFactory();

protected:
	CHandleManager< CObject, 10000 >	m_handleManager; // manager for handle to object
	CSerializableFactory< IComponent >	m_componentFactory;
};

WHITEBOX_END

#endif
