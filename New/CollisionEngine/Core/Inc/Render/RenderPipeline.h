#ifndef __RENDERPIPELINE_H__
#define __RENDERPIPELINE_H__

#include "BaseTypes.h"
#include "RenderTarget.h"
#include "Camera.h"

WHITEBOX_BEGIN

class CIndexBuffer;
class CMaterial;
class CShaderProgram;
class CMesh;

typedef std::pair< String, Matrix44 > TMatrix44Param;
typedef std::pair< String, int > TIntParam;
typedef std::pair< String, Vec3 > TVec3Param;

struct SShaderProgramParams
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
	SShaderProgramParams	shaderParams;
	bool					bCullBackFace;
	Matrix44				transformMatrix;

	size_t					index;

	ulong					key;

	bool		ComputeKey()
	{
		key = 0;		
	}
};

typedef  std::vector< SRenderUnit > TRenderQueue;


class CRenderPipeline
{
public:
	static SRenderUnit&		AddRenderUnit( TRenderQueue& renderQueue );
	static void				AddMeshToRenderQueue( CMesh* pMesh, TRenderQueue& renderQueue, const Transform& meshTransform, const Matrix34& inverseCameraMatrix, CShaderProgram* pShaderProgram, SShaderProgramParams& shaderParams, bool bCullFace );
	static void				SortRenderQueue( TRenderQueue& renderQueue );
<<<<<<< HEAD
	static void				RenderQueue( const TRenderQueue& renderQueue, IRenderTargetPtr pRenderTarget, const Matrix44& projectionMatrix, size_t& drawCalls, size_t& polyCount );
=======
	static void				RenderQueue( const TRenderQueue& renderQueue, IRenderTargetPtr pRenderTarget, const Matrix44& projectionMatrix );
>>>>>>> master

	void					Init( uint width, uint height );
	void					Render();

<<<<<<< HEAD
	size_t					GetDrawCalls() const;
	size_t					GetPolyCount() const;

	CCamera					mainCamera;
	TRenderQueue			mainRenderQueue;

private:
	size_t					m_drawCalls;
	size_t					m_polyCount;
=======
	CCamera					mainCamera;
	TRenderQueue			mainRenderQueue;
>>>>>>> master
};



WHITEBOX_END

#endif