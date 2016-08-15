﻿#include "Application.h"
#include "GlobalVariables.h"
#include "Render/Renderer.h"
#include "Render/MeshHelper.h"
#include "ResourceManager.h"

#ifndef __GEAR_VR
#include "ObjExporter.h"
#include "AssetManager.h"
#endif

#include "Render/Shader.h"
#include "Render/ShaderProgram.h"
#include "Render/Material.h"
#include "Render/Texture.h"

#include "Render/RenderPipeline.h"
#include "Render/Font.h"
#include "Render/TextMesh.h"

WHITEBOX_BEGIN

bool click = false;

CApplication::CApplication()
	: m_pRenderPipeline(nullptr){}


CMesh* quad;

CMeshPtr ezio;
CMeshPtr meca, city;

CTexturePtr ezioTexture;

CShaderProgramPtr shader, detourshader, whiteshader, textProgram;

//CFontPtr font;
//CTextMesh textMesh, textMesh2;




void CApplication::Init( uint width, uint height )
{
	m_pRenderPipeline = new CRenderPipeline();
	m_pRenderPipeline->Init( width, height );


	m_pRenderPipeline->mainCamera.transform.position = Vec3(0.0f, -50.0f, 0.0f);


#ifndef __GEAR_VR
 	CAssetManager assetManager;
 	assetManager.Export( "Assets", "Resources" );
#endif

	gVars->pResourceManager->ParseResources( "Resources" );

	ezioTexture = gVars->pResourceManager->GetResource< CTexture >("Ezio/CR_U_Ezio_Blason_DiffuseMap.dds");
	ezio = gVars->pResourceManager->GetResource< CMesh >("Ezio/Ezio.msh");
	meca = gVars->pResourceManager->GetResource< CMesh >("Vanquish/vanquish.msh");
	city = gVars->pResourceManager->GetResource< CMesh >("castle/castle.msh");

	shader = gVars->pResourceManager->GetResource< CShaderProgram >("shader.program");
	detourshader = gVars->pResourceManager->GetResource< CShaderProgram >("detour.program");
	whiteshader = gVars->pResourceManager->GetResource< CShaderProgram >("vr.program"); // white.program");

	textProgram = gVars->pResourceManager->GetResource< CShaderProgram >("text.program");




		CMaterialPtr pMat( new CMaterial() );
		pMat->m_textureLayers[0].m_pTexture = ezioTexture;


		CMeshHelper mh;

		mh.AddPosition(Vec3(0.0f, 0.0f, 0.0f));
		mh.AddPosition(Vec3(20.0f, 0.0f, 0.0f));
		mh.AddPosition(Vec3(20.0f, 0.0f, 20.0f));
		mh.AddPosition(Vec3(0.0f, 0.0f, 20.0f));
		mh.AddMeshPart();

		// 	mh.AddColor(Vec3(1, 0, 0));
		// 	mh.AddColor(Vec3(0, 1, 0));
		// 	mh.AddColor(Vec3(0, 0, 1));
		// 	mh.AddColor(Vec3(0, 0, 1));

		mh.AddUV0(Vec2(0, 0));
		mh.AddUV0(Vec2(1, 0));
		mh.AddUV0(Vec2(1, 1));
		mh.AddUV0(Vec2(0, 1));

		mh.AddNormal(Vec3(0, 1, 0));
		mh.AddNormal(Vec3(0, 1, 0));
		mh.AddNormal(Vec3(0, 1, 0));
		mh.AddNormal(Vec3(0, 1, 0));

		mh.GetMeshPart(0)->AddIndex(0);
		mh.GetMeshPart(0)->AddIndex(1);
		mh.GetMeshPart(0)->AddIndex(2);

		mh.GetMeshPart(0)->AddIndex(2);
		mh.GetMeshPart(0)->AddIndex(3);
		mh.GetMeshPart(0)->AddIndex(0);


		quad = mh.ConvertToMesh();


		quad->GetPart(0)->SetMaterial( pMat );

		//font = gVars->pResourceManager->GetResource< CFont >( "font.ttf" );





	
		gVars->pResourceManager->UpdateResourceLoading();

	//
}

void CApplication::Resize( uint width, uint height )
{
	m_pRenderPipeline->mainCamera.pRenderTarget->SetWidth( width );
	m_pRenderPipeline->mainCamera.pRenderTarget->SetHeight( height );
}

Vec2 prevPos;
float yaw = 0.0f;
float pitch = 0.0f;

float angle = 0;

void CApplication::FrameUpdate()
{
	float frameTime = gVars->pOperatingSystem->Tick();
	float fps = 1.0f / frameTime; (void)fps;

// 	CText text( String("Drawcalls : ") + ToString((int)m_pRenderPipeline->GetDrawCalls()) + String("\nPolycount : ") + ToString((int)m_pRenderPipeline->GetPolyCount()) + String("\nFramerate : ") + ToString(fps));
// 	textMesh.SetText(text, font);
// 
// 	CText txt2( U"François Fournel était\ndans la place" );
// 	textMesh2.SetText(txt2, font);


	bool bClick = gVars->pOperatingSystem->GetMouseButton(0);

	if (bClick)
	{
		if (!click)
		{
			click = true;
			prevPos = gVars->pOperatingSystem->GetMousePos();
		}
		else
		{

			Vec2 newPos = gVars->pOperatingSystem->GetMousePos();
			Vec2 delta = newPos - prevPos;
			
			delta.x *= -200.0f / (float)m_pRenderPipeline->mainCamera.pRenderTarget->GetWidth();
			delta.y *= -200.0f / (float)m_pRenderPipeline->mainCamera.pRenderTarget->GetHeight();

			yaw += delta.x;
			pitch += delta.y;

			m_pRenderPipeline->mainCamera.transform.rotation = Quat( Degree(yaw), Degree(pitch), Degree(0.0f) );

			prevPos = newPos;

		}
	}
	else
	{
		click = false;
	}


	if ( gVars->pOperatingSystem->IsPressingKey( Key::D ) )
	{
		m_pRenderPipeline->mainCamera.transform.position += m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Right * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::Q ) )
	{
		m_pRenderPipeline->mainCamera.transform.position += m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Left * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::Z ) )
	{
		m_pRenderPipeline->mainCamera.transform.position += m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Forward * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::S ) )
	{
		m_pRenderPipeline->mainCamera.transform.position += m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Backward * 500.0f * frameTime;
	}



#ifdef __GEAR_VR
	return;
#endif




	Transform textTransf;
	
	textTransf = m_pRenderPipeline->mainCamera.transform;
	textTransf.position = textTransf.position + m_pRenderPipeline->mainCamera.transform.rotation * Vec3(-500.0f, 1000.0f, 0.0f);
	//textTransf.rotation = Quat();




	Vec3 lightDirection = m_pRenderPipeline->mainCamera.transform.TransformVector(Vec3(1.0f, 1.0f, 0.0f));
	lightDirection = m_pRenderPipeline->mainCamera.transform.GetInverse().TransformVector(lightDirection);
	lightDirection = Vec3(lightDirection.x, lightDirection.z, -lightDirection.y);
	lightDirection.Normalize();

	{
		SShaderProgramParams2 shaderParams;
		shaderParams.intParams.push_back(TIntParam("shaderTexture", 0));
		shaderParams.vec3Params.push_back(TVec3Param("lightDirection", lightDirection));

		//CRenderPipeline::AddMeshToRenderQueue(quad, m_pRenderPipeline->mainRenderQueue, Transform(), m_pRenderPipeline->mainCamera.inverseTransformMatrix, shader.get(), shaderParams, true);
	}


	if (city)
	{
		Transform t;
		t.position = Vec3(-300.0f, 20.0f, 0.0f);
		//t.rotation = Quat(Degree(45.0f), Degree(0), Degree(0.0f));


		t.scale = Vec3(10.0f, 10.0f, 10.0f);
		t.rotation = t.rotation * Quat::CreateRotX(Degree(90.0f));


		SShaderProgramParams2 shaderParams;
		shaderParams.intParams.push_back(TIntParam("shaderTexture", 0));
		shaderParams.vec3Params.push_back(TVec3Param("lightDirection", lightDirection));

	//	CRenderPipeline::AddMeshToRenderQueue(city.get(), m_pRenderPipeline->mainRenderQueue, t, m_pRenderPipeline->mainCamera.inverseTransformMatrix, shader.get(), shaderParams, true);

	}


	Transform t;
	t.position = Vec3(300.0f, 20.0f, 0.0f);
//	t.rotation = Quat(Degree(45.0f), Degree(0), Degree(0.0f));

	angle += frameTime * 30.0f;
	
	t.scale = Vec3(100.0f, 100.0f, 100.0f);
	t.rotation = t.rotation * Quat::CreateRotZ(Degree(angle)) * Quat::CreateRotX(Degree(90.0f));


	for (int i = 0; i < 10; ++i)
	{
		Transform t2 = t;
		t2.position.x += 200.0f * i;
		t2.scale = t2.scale * ((float)i / 5.0f);

		SShaderProgramParams2 shaderParams;
		shaderParams.intParams.push_back( TIntParam( "shaderTexture", 0 ) );
		shaderParams.vec3Params.push_back( TVec3Param( "lightDirection", lightDirection ) );


		if (i < 5)
		{
			for (size_t j = 0; j < ezio->GetPartCount(); ++j)
			{
				CShaderUniformsValues& uniforms = CRenderPipeline::AddRenderProxyToQueue(ezio.get(), j, m_pRenderPipeline->proxies, t2, shader.get(), true);
			//	uniforms.SetUniformValue<int>(shader.get(), "shaderTexture", 0);
				//uniforms.SetUniformValue<Vec3>(shader.get(), "lightDirection", lightDirection);
			}


			//CRenderPipeline::AddMeshToRenderQueue( ezio.get(), m_pRenderPipeline->mainRenderQueue, t2, m_pRenderPipeline->mainCamera.inverseTransformMatrix, shader.get(), shaderParams, true );
		}
		else
		{
			for (size_t j = 0; j < meca->GetPartCount(); ++j)
			{
				CShaderUniformsValues& uniforms = CRenderPipeline::AddRenderProxyToQueue(meca.get(), j, m_pRenderPipeline->proxies, t2, shader.get(), true);
				//uniforms.SetUniformValue<int>(shader.get(), "shaderTexture", 0);
				//uniforms.SetUniformValue<Vec3>(shader.get(), "lightDirection", lightDirection);
			}

			//CRenderPipeline::AddMeshToRenderQueue( meca.get(), m_pRenderPipeline->mainRenderQueue, t2, m_pRenderPipeline->mainCamera.inverseTransformMatrix, whiteshader.get(), shaderParams, true );
			
			SShaderProgramParams2 params;
			//CRenderPipeline::AddMeshToRenderQueue( meca.get(), m_pRenderPipeline->mainRenderQueue, t2, m_pRenderPipeline->mainCamera.inverseTransformMatrix, detourshader.get(), params, false );
		}
	}

	lightDirection = m_pRenderPipeline->mainCamera.transform.GetInverse().TransformVector(lightDirection);
	lightDirection = Vec3(lightDirection.x, lightDirection.z, -lightDirection.y);
	lightDirection.Normalize();

	{

		static float a = 0.0f;
		a = a + 30.0f * frameTime;
		Transform tt;
		tt.rotation = Quat::CreateRotZ( Degree(0.0f) );

		CRenderPipeline::AddRenderProxyToQueue(quad, 0, m_pRenderPipeline->proxies, tt, shader.get(), true);


		//CRenderPipeline::AddMeshToRenderQueue(quad, m_pRenderPipeline->mainRenderQueue, Transform(), m_pRenderPipeline->mainCamera.inverseTransformMatrix, shader.get(), shaderParams, true);

		//	CRenderPipeline::AddMeshToRenderQueue(const_cast<CMesh*>(textMesh.GetMesh().get()), m_pRenderPipeline->mainRenderQueue, textTransf, m_pRenderPipeline->mainCamera.inverseTransformMatrix, textProgram.get(), shaderParams, true);
	//	CRenderPipeline::AddMeshToRenderQueue(const_cast<CMesh*>(textMesh2.GetMesh().get()), m_pRenderPipeline->mainRenderQueue, Transform(), m_pRenderPipeline->mainCamera.inverseTransformMatrix, textProgram.get(), shaderParams, true);

	}


	m_pRenderPipeline->Render();
}

WHITEBOX_END
