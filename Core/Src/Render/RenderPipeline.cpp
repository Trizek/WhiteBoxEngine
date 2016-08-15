#include "Render/RenderPipeline.h"
#include "Render/ShaderProgram.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"
#include "Render/Material.h"
#include "Render/Mesh.h"

WHITEBOX_BEGIN

SRenderUnit&	CRenderPipeline::AddRenderUnit( TRenderQueue2& renderQueue )
{
	renderQueue.push_back( SRenderUnit( renderQueue.size() ) );
	return renderQueue.back();
}

void	CRenderPipeline::AddMeshToRenderQueue( CMesh* pMesh, TRenderQueue2& renderQueue, const Transform& meshTransform, const Matrix34& inverseCameraMatrix, CShaderProgram* pShaderProgram, SShaderProgramParams2& shaderParams, bool bCullBackFace )
{
	for( size_t iPart = 0 ; iPart < pMesh->GetPartCount() ; ++iPart )
	{
		CMesh::CPart* pPart = pMesh->GetPart( iPart );
		SRenderUnit& renderUnit = AddRenderUnit( renderQueue );

		renderUnit.pIndexBuffer = pPart->GetIndexBuffer();
		renderUnit.pMaterial = pPart->GetMaterial().get();
		renderUnit.pShaderProgram = pShaderProgram;
		renderUnit.shaderParams = shaderParams;
		renderUnit.bCullBackFace = bCullBackFace;

		
		Matrix34 transformMatrix; transformMatrix.FromTransform( meshTransform );
		transformMatrix = inverseCameraMatrix * transformMatrix;

		gVars->pRenderer->FormatTransformMatrix( transformMatrix, renderUnit.transformMatrix );
	}
}




void			CRenderPipeline::SortRenderQueue( TRenderQueue2& renderQueue )
{

}

void			CRenderPipeline::RenderQueue( const TRenderQueue2& renderQueue, IRenderTargetPtr pRenderTarget, const Matrix44& projectionMatrix, size_t& drawCalls, size_t& polyCount )
{
	drawCalls = renderQueue.size();
	polyCount = 0;


	const SRenderUnit* pLastRenderUnit = nullptr;
	for (size_t i = 0; i < renderQueue.size(); ++i)
	{
		const SRenderUnit& renderUnit = renderQueue[ renderQueue[ i ].index ];

		gVars->pRenderer->BindIndexBuffer( renderUnit.pIndexBuffer->GetBufferId(), renderUnit.pIndexBuffer->GetIndexCount() );

		renderUnit.pShaderProgram->Bind(true);

		gVars->pRenderer->SetUniformMatrix44( renderUnit.pShaderProgram->GetProgramId(), "projectionMatrix", projectionMatrix );
		gVars->pRenderer->SetUniformMatrix44( renderUnit.pShaderProgram->GetProgramId(), "modelViewMatrix", renderUnit.transformMatrix );

		gVars->pRenderer->CullFace( renderUnit.bCullBackFace );

		if ( renderUnit.pMaterial != nullptr )
		{
			for (size_t iTex = 0; iTex < CMaterial::MAX_TEXTURE_LAYER; ++iTex)
			{
				CTexturePtr pTexture = renderUnit.pMaterial->m_textureLayers[iTex].m_pTexture;
				if (!pTexture)
				{
					break;
				}

				gVars->pRenderer->BindTexture(pTexture->GetTextureId(), iTex);

// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

// 
//   				glEnable(GL_BLEND);
// 				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			}
		}

		for( const TIntParam& intParam : renderUnit.shaderParams.intParams )
		{
			gVars->pRenderer->SetUniformInt( renderUnit.pShaderProgram->GetProgramId(), intParam.first, intParam.second );
		}
		for( const TVec3Param& vec3Param : renderUnit.shaderParams.vec3Params )
		{
			gVars->pRenderer->SetUniformVec3( renderUnit.pShaderProgram->GetProgramId(), vec3Param.first, vec3Param.second );
		}
		for( const TMatrix44Param& matrix44Param : renderUnit.shaderParams.matrix44Params )
		{
			gVars->pRenderer->SetUniformMatrix44( renderUnit.pShaderProgram->GetProgramId(), matrix44Param.first, matrix44Param.second );
		}

		gVars->pRenderer->RenderBoundTriangles( renderUnit.pIndexBuffer->GetIndexCount() );

		pLastRenderUnit = &renderUnit;

		polyCount += renderUnit.pIndexBuffer->GetIndexCount();
	}
}


CShaderUniformsValues&		CRenderPipeline::AddRenderProxyToQueue( CMesh* pMesh, size_t part, TRenderProxies& renderProxies, const Transform& meshTransform, CShaderProgram* pShaderProgram, bool bCullBackFace )
{
	renderProxies.push_back(SRenderProxy());
	SRenderProxy& proxy = renderProxies.back();

	CMesh::CPart* pPart = pMesh->GetPart( part );

	proxy.pIndexBuffer = pPart->GetIndexBuffer();
	proxy.pMaterial = pPart->GetMaterial().get();
	proxy.pShaderProgram = pShaderProgram;
	proxy.transformMatrix.FromTransform( meshTransform );

	proxy.bCullBackFace = bCullBackFace;
	
	return proxy.uniformValues;
}

void	CRenderPipeline::RenderQueue( TRenderProxies& renderProxies, IRenderTargetPtr pRenderTarget, const Matrix34& invCamMatrix, const Matrix44& projectionMatrix, size_t& drawCalls, size_t& polyCount )
{
	drawCalls = renderProxies.size();
	polyCount = 0;
	

	Matrix44* matrices = (Matrix44*)gVars->pRenderer->LockUniformBuffer( m_sceneMatUniformBufferId, 0, sizeof(Matrix44) );
	matrices[0] = projectionMatrix;
	gVars->pRenderer->UnlockUniformBuffer();

	Vec3 lightDir(1.0f, 0.0f, 0.0f);


	Vec3* pLightDir = (Vec3*)gVars->pRenderer->LockUniformBuffer( m_lightUniformBufferId, 0, sizeof(Vec3) );
	*pLightDir = lightDir;
	gVars->pRenderer->UnlockUniformBuffer();

	SRenderProxy* pLastRenderProxy = nullptr;
	for ( size_t i = 0; i < renderProxies.size(); ++i )
	{
		SRenderProxy& renderProxy = renderProxies[ i ];

		gVars->pRenderer->BindIndexBuffer( renderProxy.pIndexBuffer->GetBufferId(), renderProxy.pIndexBuffer->GetIndexCount() );

		renderProxy.pShaderProgram->Bind(true);

		// Uniforms
 		Matrix44 transformMatrix;
 		gVars->pRenderer->FormatTransformMatrix( invCamMatrix * renderProxy.transformMatrix, transformMatrix);
 		renderProxy.uniformValues.SetUniformValue< Matrix44 >( renderProxy.pShaderProgram, "modelViewMatrix", transformMatrix );
	
		renderProxy.uniformValues.SetUniformValue< void* >(renderProxy.pShaderProgram, "SceneMatrices", m_sceneMatUniformBufferId);
		renderProxy.uniformValues.SetUniformValue< void* >(renderProxy.pShaderProgram, "Lighting", m_lightUniformBufferId);
		
		renderProxy.uniformValues.ApplyValues( renderProxy.pShaderProgram );

		gVars->pRenderer->CullFace( renderProxy.bCullBackFace );

		if ( renderProxy.pMaterial != nullptr )
		{
			for (size_t iTex = 0; iTex < CMaterial::MAX_TEXTURE_LAYER; ++iTex)
			{
				CTexturePtr pTexture = renderProxy.pMaterial->m_textureLayers[iTex].m_pTexture;
				if (!pTexture)
				{
					break;
				}

				gVars->pRenderer->BindTexture( pTexture->GetTextureId(), iTex );

// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

// 
//   			glEnable(GL_BLEND);
// 				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			}
		}

		gVars->pRenderer->RenderBoundTriangles( renderProxy.pIndexBuffer->GetIndexCount() );

		pLastRenderProxy = &renderProxy;

		polyCount += renderProxy.pIndexBuffer->GetIndexCount();
	}
}


void* pFontId;
void			CRenderPipeline::Init( uint width, uint height )
{
	mainCamera.pRenderTarget = gVars->pRenderer->CreateRenderWindow( 0, 0, width, height );


	m_sceneMatUniformBufferId = gVars->pRenderer->CreateUniformBuffer( sizeof(Matrix44) * 2 );
	m_lightUniformBufferId = gVars->pRenderer->CreateUniformBuffer( sizeof(Vec3)  );
}

void			CRenderPipeline::Render()
{
	gVars->pRenderer->StartRenderFrame();
	gVars->pRenderer->BindRenderWindow( static_cast<CRenderWindow*>( mainCamera.pRenderTarget.get() ) );

	mainCamera.ComputeProjectionMatrix();
	mainCamera.ComputeInverseTransformMatrix();

 	CRenderPipeline::RenderQueue( mainRenderQueue, mainCamera.pRenderTarget, mainCamera.projectionMatrix, m_drawCalls, m_polyCount );
 	mainRenderQueue.clear();

	RenderQueue(proxies, mainCamera.pRenderTarget, mainCamera.inverseTransformMatrix, mainCamera.projectionMatrix, m_drawCalls, m_polyCount);
	proxies.clear();


	gVars->pRenderer->UnbindIndexBuffer();
	gVars->pRenderer->BindProgram( nullptr );
	gVars->pRenderer->UnbindTexture( 0 );


}


CRenderPipeline::~CRenderPipeline()
{
	gVars->pRenderer->DestroyUniformBuffer( m_sceneMatUniformBufferId );

	gVars->pRenderer->DestroyUniformBuffer(m_lightUniformBufferId);
}


size_t			CRenderPipeline::GetDrawCalls() const
{
	return m_drawCalls;
}

size_t			CRenderPipeline::GetPolyCount() const
{
	return m_polyCount;
}

WHITEBOX_END