#ifndef __SKINNEDMESHRENDERNODE_H__
#define __SKINNEDMESHRENDERNODE_H__

#include "MeshRenderNode.h"
#include "Animation/Skeleton.h"

WHITEBOX_BEGIN

class CSkinnedMeshRenderNode : public CMeshRenderNode
{
DECLARE_SERIALIZABLE_CLASS(CSkinnedMeshRenderNode)

public:
	CSkinnedMeshRenderNode() = default;
	CSkinnedMeshRenderNode( const String& _name ) : CMeshRenderNode(_name) {}

	virtual void	Refresh() override;
	void			ApplySkinningPose( const CPose& skinningPose );

	virtual void Serialize(ISerializer& serializer)
	{
		CMeshRenderNode::Serialize(serializer);
	}

private:
	void*	m_skinMatricesBufferId;
};

DEFINE_SMART_PTR(CSkinnedMeshRenderNode)

WHITEBOX_END

#endif
