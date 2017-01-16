
#include "Render/SkinnedMeshRenderNode.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"
#include "Animation/Skeleton.h"
#include "Render/RenderPipeline.h"
#include "Engine.h"

WHITEBOX_BEGIN

void CSkinnedMeshRenderNode::Refresh()
{
	CMeshRenderNode::Refresh();

	m_skinMatricesBufferId = gVars->pRenderer->CreateUniformBuffer( sizeof(Matrix44) * 64 );


	for( size_t i = 0; i < m_renderProxyIndices.size(); ++ i)
	{
		size_t proxyIndex = m_renderProxyIndices[ i ];

		SRenderProxy& renderProxy = gVars->pEngine->m_pRenderPipeline->m_proxies[ proxyIndex ];
		renderProxy.uniformValues.SetUniformValue< void* >( GET_CLAMPED_ELEM(m_shaderPrograms, i), "SkinningMatrices", m_skinMatricesBufferId ); 
	}
}

void	CSkinnedMeshRenderNode::ApplySkinningPose( const CPose& skinningPose )
{
	Matrix44* matrices = (Matrix44*)gVars->pRenderer->LockUniformBuffer( m_skinMatricesBufferId, 0, 64 * sizeof(Matrix44) );
	for (int i = 0; i < Min<int>(skinningPose.m_boneTransforms.size(), 64); ++i)
	{
		Matrix34 m;
		m.FromTransform( skinningPose.m_boneTransforms[i] );

		matrices[i] = Matrix44(m);
	}

	gVars->pRenderer->UnlockUniformBuffer();
}

WHITEBOX_END

