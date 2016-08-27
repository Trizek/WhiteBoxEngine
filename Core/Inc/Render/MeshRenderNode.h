#ifndef __MESHRENDERNODE_H__
#define __MESHRENDERNODE_H__

#include "SceneNode.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#define GET_CLAMPED_ELEM(container, index) ((container.size() > 0)? container[ Min<size_t>(index, container.size() - 1) ].get() : nullptr)

WHITEBOX_BEGIN

class CMeshRenderNode : public CSpatialNode
{
public:
	void			SetMesh( CMeshPtr pMesh );
	void			SetShaderProgram( size_t index, CShaderProgramPtr pShaderProgram );
	virtual void	PropagateTransform( const Transform& globalParentTransform ) override;
	
	virtual void	Refresh() override;
	
protected:
	CMeshPtr							m_pMesh;
	std::vector< CShaderProgramPtr >	m_shaderPrograms;

	std::vector< size_t >				m_renderProxyIndices;
};

DEFINE_SMART_PTR(CMeshRenderNode)

WHITEBOX_END

#endif
