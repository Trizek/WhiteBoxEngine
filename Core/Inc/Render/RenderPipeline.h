#ifndef __RENDERPIPELINE_H__
#define __RENDERPIPELINE_H__

#include "BaseTypes.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "GlobalVariables.h"
#include "Renderer.h"

WHITEBOX_BEGIN

class CIndexBuffer;
class CMaterial;
class CShaderProgram;
class CMesh;



typedef std::pair< String, Matrix44 > TMatrix44Param;
typedef std::pair< String, int > TIntParam;
typedef std::pair< String, Vec3 > TVec3Param;

struct SShaderProgramParams2
{
	std::vector< TIntParam >			intParams;
	std::vector< TVec3Param >			vec3Params;
	std::vector< TMatrix44Param >		matrix44Params;
};

struct SRenderUnit
{
	SRenderUnit( size_t arrayIndex ) : index(arrayIndex){}

	CIndexBuffer*			pIndexBuffer;
	CMaterial*				pMaterial;
	CShaderProgram*			pShaderProgram;
	SShaderProgramParams2	shaderParams;
	bool					bCullBackFace;
	Matrix44				transformMatrix;

	size_t					index;

	ulong					key;
};

class CShaderUniformsValues
{
public:
	template< class Type >
	void	GetUniformValue( const CShaderProgram* pProgram, const String& name, Type& value )
	{
		const SUniformInfo* pUniformInfo = pProgram->GetUniformInfo( name );
		if ( pUniformInfo != nullptr )
		{
			const void* pSource = m_data + pUniformInfo->offset;

			switch (pUniformInfo->type)
			{
			case EUniformType::Int:			*((int*)&value) = *((int*)pSource); break;
			case EUniformType::Vector3:		*((Vec3*)&value) = *((Vec3*)pSource); break;
			case EUniformType::Matrix4x4:	*((Matrix44*)&value) = *((Matrix44*)pSource); break;
			case EUniformType::Buffer:		*((void**)&value) = *((void**)pSource); break;
			}			
		}
	}

	template< class Type >
	void	SetUniformValue( const CShaderProgram* pProgram, const String& name, const Type& value )
	{
		const SUniformInfo* pUniformInfo = pProgram->GetUniformInfo( name );
		if ( pUniformInfo != nullptr )
		{
			void* pDest = m_data + pUniformInfo->offset;

			switch (pUniformInfo->type)
			{
			case EUniformType::Int:			*((int*)pDest) = *((int*)&value); break;
			case EUniformType::Vector3:		*((Vec3*)pDest) = *((Vec3*)&value); break;
			case EUniformType::Matrix4x4:	*((Matrix44*)pDest) = *((Matrix44*)&value); break;
			case EUniformType::Buffer:		*((void**)pDest) = *((void**)&value); break;
			}			
		}
	}

	void	ApplyValues( const CShaderProgram* pProgram ) const
	{
		for( size_t i = 0; i < pProgram->GetUniformCount(); ++i )
		{
			const SUniformInfo& uniformInfo = pProgram->GetUniformInfo(i);
			
			const void* pSource = m_data + uniformInfo.offset;
			switch (uniformInfo.type)
			{
			case EUniformType::Int:			gVars->pRenderer->SetUniformInt( pProgram->GetProgramId(), uniformInfo.location, *((const int*)pSource) ); break;
			case EUniformType::Vector3:		gVars->pRenderer->SetUniformVec3( pProgram->GetProgramId(), uniformInfo.location, *((const Vec3*)pSource) ); break;
			case EUniformType::Matrix4x4:	gVars->pRenderer->SetUniformMatrix44( pProgram->GetProgramId(), uniformInfo.location, *((const Matrix44*)pSource) ); break;
			case EUniformType::Buffer:		
			{
				typedef void* BufferType;
				BufferType const* pData = (BufferType const*)pSource;
				BufferType bufferId = *pData;
				if ( bufferId != (BufferType)0 )
				{
					gVars->pRenderer->BindUniformBuffer( bufferId, uniformInfo.binding ); break;
				}
				break;
			}
			}
		}
	}

private:
	char			m_data[ SUniformInfo::MAX_UNIFORMS_SIZE ];
};


struct SRenderProxy
{
	CIndexBuffer*			pIndexBuffer;
	CMaterial*				pMaterial;
	CShaderProgram*			pShaderProgram;
	CShaderUniformsValues	uniformValues;
	
	bool					bCullBackFace;

	Matrix34				transformMatrix;
};


typedef  std::vector< SRenderUnit > TRenderQueue2;

typedef std::vector< SRenderProxy >	TRenderProxies;

class CRenderPipeline
{
public:
	~CRenderPipeline();

	static SRenderUnit&		AddRenderUnit( TRenderQueue2& renderQueue );
	static void				AddMeshToRenderQueue( CMesh* pMesh, TRenderQueue2& renderQueue, const Transform& meshTransform, const Matrix34& inverseCameraMatrix, CShaderProgram* pShaderProgram, SShaderProgramParams2& shaderParams, bool bCullFace );
	static void				SortRenderQueue( TRenderQueue2& renderQueue );
	static void				RenderQueue( const TRenderQueue2& renderQueue, IRenderTargetPtr pRenderTarget, const Matrix44& projectionMatrix, size_t& drawCalls, size_t& polyCount );

	static CShaderUniformsValues&		AddRenderProxyToQueue( CMesh* pMesh, size_t part, TRenderProxies& renderProxies, const Transform& meshTransform, CShaderProgram* pShaderProgram, bool bCullFace );
	void				RenderQueue( TRenderProxies& renderProxies, IRenderTargetPtr pRenderTarget, const Matrix34& invCamMatrix, const Matrix44& projectionMatrix, size_t& drawCalls, size_t& polyCount );

	void					Init( uint width, uint height );
	void					Render();

	size_t					GetDrawCalls() const;
	size_t					GetPolyCount() const;

	CCamera					mainCamera;
	TRenderQueue2			mainRenderQueue;

	TRenderProxies			proxies;

//private:
	size_t					m_drawCalls;
	size_t					m_polyCount;

	void*					m_sceneMatUniformBufferId;
	void*					m_lightUniformBufferId;
};



WHITEBOX_END

#endif