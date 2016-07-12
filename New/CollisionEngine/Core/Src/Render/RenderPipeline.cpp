#include "Render/RenderPipeline.h"
#include "Render/ShaderProgram.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"
#include "Render/Material.h"
#include "Render/Mesh.h"
#include "GL/glew.h"

/*#include "drawtext.h"*/

WHITEBOX_BEGIN

SRenderUnit&	CRenderPipeline::AddRenderUnit( TRenderQueue& renderQueue )
{
	renderQueue.push_back( SRenderUnit( renderQueue.size() ) );
	return renderQueue.back();
}

void	CRenderPipeline::AddMeshToRenderQueue( CMesh* pMesh, TRenderQueue& renderQueue, const Transform& meshTransform, const Matrix34& inverseCameraMatrix, CShaderProgram* pShaderProgram, SShaderProgramParams& shaderParams, bool bCullBackFace )
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

		int x = 2;
	}
}


void			CRenderPipeline::SortRenderQueue( TRenderQueue& renderQueue )
{

}

void			CRenderPipeline::RenderQueue( const TRenderQueue& renderQueue, IRenderTargetPtr pRenderTarget, const Matrix44& projectionMatrix )
{
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
	}
}

void* pFontId;
void			CRenderPipeline::Init( uint width, uint height )
{
	mainCamera.pRenderTarget = gVars->pRenderer->CreateRenderWindow( 0, 0, width, height );

	pFontId = gVars->pRenderer->LoadFont( "font.ttf", 24 );
	gVars->pRenderer->UseFont( pFontId, 24 );

	glEnable(GL_CULL_FACE);
}

void			CRenderPipeline::Render()
{
	gVars->pRenderer->StartRenderFrame();
	gVars->pRenderer->BindRenderWindow( static_cast<CRenderWindow*>( mainCamera.pRenderTarget.get() ) );

	mainCamera.ComputeProjectionMatrix();
	mainCamera.ComputeTransformMatrix();

	glMatrixMode(GL_MODELVIEW);                     // Select The Modelview Matrix
	glLoadIdentity();

	CRenderPipeline::RenderQueue( mainRenderQueue, mainCamera.pRenderTarget, mainCamera.projectionMatrix );
	mainRenderQueue.clear();

	gVars->pRenderer->UnbindIndexBuffer();
	gVars->pRenderer->BindProgram( nullptr );
	gVars->pRenderer->UnbindTexture( 0 );

	

	// Black text
	glColor3f(0.0f, 0.0f, 0.0f);

	// Set proj matrix to screen space
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, mainCamera.pRenderTarget->GetWidth(), 0, mainCamera.pRenderTarget->GetHeight(), -1, 1);

	// Reset model matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		glPushMatrix();

		glTranslatef(100.0f, 100.0f, 0.0f);
// 		dtx_string("Francois Fournel");

		glPopMatrix();


/*		dtx_flush();*/


}

WHITEBOX_END