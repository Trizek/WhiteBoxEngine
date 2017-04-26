
#include "Render/MeshRenderNode.h"
#include "Render/RenderPipeline.h"
#include "Engine.h"


WHITEBOX_BEGIN

DEFINE_SERIALIZABLE_CLASS(CMeshRenderNode);


void	CMeshRenderNode::SetMesh( CMeshPtr pMesh )
{
	m_pMesh = pMesh;
}

void	CMeshRenderNode::SetShaderProgram( size_t index, CShaderProgramPtr pShaderProgram )
{
	if ( index >= m_shaderPrograms.size() )
	{
		m_shaderPrograms.resize( index + 1 );
	}

	m_shaderPrograms[ index ] = pShaderProgram;
}

void	CMeshRenderNode::PropagateTransform( const Transform& globalParentTransform )
{
	CSpatialNode::PropagateTransform( globalParentTransform );

	Transform transform;
	GetGlobalTransform( transform );

	for( size_t proxyIndex : m_renderProxyIndices )
	{
		SRenderProxy& renderProxy = gVars->pEngine->m_pRenderPipeline->m_proxies[ proxyIndex ];
		renderProxy.transformMatrix.FromTransform( transform );
	}
}

void	CMeshRenderNode::Refresh()
{
	Transform transform;
	GetGlobalTransform( transform );

	for ( size_t i = 0; i < m_pMesh->GetPartCount(); ++i )
	{
		SRenderProxy& renderProxy = gVars->pEngine->m_pRenderPipeline->AddRenderProxy();

		CMesh::CPart* pPart = m_pMesh->GetPart( i );
		renderProxy.pIndexBuffer = pPart->GetIndexBuffer();
		renderProxy.pMaterial = pPart->GetMaterial().get();
		renderProxy.pShaderProgram = GET_CLAMPED_ELEM( m_shaderPrograms, i );
		renderProxy.transformMatrix.FromTransform( transform );
		renderProxy.bCullBackFace = true;

		m_renderProxyIndices.push_back( renderProxy.index );
	}

	CSceneNode::Refresh();
}

void	CMeshRenderNode::Serialize( ISerializer& serializer )
{
	CSpatialNode::Serialize( serializer );

	serializer.Value<CMeshPtr>( "mesh", m_pMesh );

	if ( serializer.IsReading() )
	{
		if ( serializer.BeginGroup("shaderPrograms") )
		{
			if ( serializer.BeginGroup("Program") )
			{
				CShaderProgramPtr pProgram;
				serializer.Value<CShaderProgramPtr>( "name", pProgram );
				m_shaderPrograms.push_back( pProgram );
				serializer.EndGroup();
			}

			serializer.EndGroup();
		}
	}
	else
	{
		if ( serializer.BeginGroup("shaderPrograms") )
		{
			for( CShaderProgramPtr& pProgram : m_shaderPrograms )
			{
				serializer.BeginGroup("Program");
				serializer.Value<CShaderProgramPtr>( "name", pProgram );
				serializer.EndGroup();
			}

			serializer.EndGroup();
		}
	}
}



WHITEBOX_END

