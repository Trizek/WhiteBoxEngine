#ifdef __GEAR_VR

#include <android/log.h>


#include "System/GearVR/WhiteBoxGearLibrary.h"

#include "BaseTypes.h"
#include "GlobalVariables.h"
#include "Render/Mesh.h"
#include "Render/MeshHelper.h"
#include "Render/ShaderProgram.h"
#include "Render/Renderer.h"

#define C_FUNCS_BEGIN extern "C"{
#define C_FUNCS_END }

WhiteBox::CMeshPtr pMesh;
WhiteBox::CShaderProgramPtr pProg;

WhiteBox::CShaderPtr vert, pix;

class MaClasse
{
public:
	int MaFonction()
	{
		return 250;
	}
};

using namespace WhiteBox;

C_FUNCS_BEGIN

int	GetVal()
{
	MaClasse mc;
	return mc.MaFonction();
}


void WBGL_Update()
{
	gVars->pRenderer->BindProgram( pProg->GetProgramId() );
	 gVars->pRenderer->BindIndexBuffer( pMesh->GetPart(0)->GetIndexBuffer()->GetBufferId(), pMesh->GetPart(0)->GetIndexBuffer()->GetIndexCount());

	gVars->pRenderer->RenderBoundTriangles(pMesh->GetPart(0)->GetIndexBuffer()->GetIndexCount());

}

void WBGL_Init()
{





	SGlobalVariables::Init();


 	CMeshHelper mh;
 
 	mh.AddPosition(Vec3(0.0f, 0.0f, -200.0f));
 	mh.AddPosition(Vec3(200.0f, 0.0f, -200.0f));
 	mh.AddPosition(Vec3(200.0f, 200.0f, -200.0f));
 	mh.AddPosition(Vec3(0.0f, 200.0f, -200.0f));
 	mh.AddMeshPart();
 
//  	mh.AddUV0(Vec2(0, 0));
//  	mh.AddUV0(Vec2(1, 0));
//  	mh.AddUV0(Vec2(1, 1));
//  	mh.AddUV0(Vec2(0, 1));
//  
//  	mh.AddNormal(Vec3(0, 1, 0));
//  	mh.AddNormal(Vec3(0, 1, 0));
//  	mh.AddNormal(Vec3(0, 1, 0));
//  	mh.AddNormal(Vec3(0, 1, 0));
 
 	mh.GetMeshPart(0)->AddIndex(0);
 	mh.GetMeshPart(0)->AddIndex(1);
 	mh.GetMeshPart(0)->AddIndex(2);
 
 	mh.GetMeshPart(0)->AddIndex(2);
 	mh.GetMeshPart(0)->AddIndex(3);
 	mh.GetMeshPart(0)->AddIndex(0);
 
 	pMesh = mh.ConvertToMesh();


	char const ps[] = 	"\nin lowp vec4 fragmentColor;\n"
	"\nout lowp vec4 outputColor;"
	"\nvoid main(void)"
	"\n{"
		"\noutputColor = vec4(0.0, 1.0, 0.0, 1.0);"
	"\n}";

	char const vs[] = "in vec3 inputPosition;"
		"\n#if __GEAR_VR"
		"\nuniform SceneMatrices"
		"\n{"
		"\nuniform mat4 ViewMatrix[NUM_VIEWS];"
		"\nuniform mat4 ProjectionMatrix;"
		"\n} sm;"
		"\n#else"
		"\nuniform mat4 modelViewMatrix;"
		"\nuniform mat4 projectionMatrix;"
		"\n#endif"
		"\nout vec4 fragmentColor;\n"
		"\nvoid main(void)"
		"\n{"
		"\n#if __GEAR_VR"
		"\ngl_Position = sm.ProjectionMatrix * (sm.ViewMatrix[VIEW_ID] * (vec4(inputPosition, 1.0)));"
		"\n#else"
		"\ngl_Position = modelViewMatrix * vec4(inputPosition, 1.0f);"
		"\ngl_Position = projectionMatrix * gl_Position;"
		"\nfragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
		"\n#endif"
		"\n}";

	String err;

	void* psId = gVars->pRenderer->CreateShader( ps, EShaderType::PS );	
	void* vsId = gVars->pRenderer->CreateShader( vs, EShaderType::VS );

	

	pProg = new CShaderProgram();
	pProg->AddShader(new CShader(EShaderType::PS, psId));
	pProg->AddShader(new CShader(EShaderType::VS, vsId));
	
	if ( !gVars->pRenderer->CompileShader( psId, err ) || !gVars->pRenderer->CompileShader( vsId, err ) ||  !pProg->LinkProgram( err ) )
	{
		__android_log_print(ANDROID_LOG_ERROR, "VrCube", "Erreur : %s\n", err.c_str() );
 
 		SGlobalVariables* p = nullptr;
 		p->pFontSystem = nullptr;
	}
}

C_FUNCS_END

#endif