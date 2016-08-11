#ifndef __MESHRENDERCOMPONENT_H__
#define __MESHRENDERCOMPONENT_H__

#include "Object.h"

WHITEBOX_BEGIN

class CMeshRenderComponent : public IComponent
{
	DECLARE_SERIALIZABLE_FACTORY( CMeshRenderComponent )
	
public:	
	static const IComponent::EComponent Type = IComponent::eC_MeshRender;

	virtual void Serialize( ISerializer& serializer );
	virtual EComponent	GetType() const;
	
	const String&	GetMeshName() const;
	void	SetMeshName( const String& meshName );
	
private:
	virtual void	Load();
	virtual void	Unload();

	String m_meshName;
	
	IRenderMeshPtr m_pRenderMesh;
};

WHITEBOX_END

#endif
