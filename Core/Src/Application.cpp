#include "Application.h"
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
#include "Animation/Skeleton.h"

WHITEBOX_BEGIN

bool click = false;

CApplication::CApplication()
	: m_pRenderPipeline(nullptr){}


CMesh* quad;
CMeshPtr ezio;
CMeshPtr meca, city;

CTexturePtr ezioTexture;

CShaderProgramPtr shader, detourshader, whiteshader, textProgram, skin;

CFontPtr font;
CTextMesh textMesh, textMesh2;


CMeshPtr q;
CShaderProgramPtr pProg;

CTimer timer;

CSkeletonPtr skel;

void CApplication::InitApplication( uint width, uint height )
{

	CDataStream stream;
	gVars->pOperatingSystem->GetDataStream("toto.txt", stream);




	m_pRenderPipeline = new CRenderPipeline();
	m_pRenderPipeline->Init( width, height );


	m_pRenderPipeline->mainCamera.transform.position = Vec3(0.0f, -20.0f, 0.0f);


#ifndef __GEAR_VR
 	CAssetManager assetManager;
 	assetManager.Export( "../../../Assets", "" );
#endif

	gVars->pResourceManager->ParseResources( "" );

	//ezioTexture = gVars->pResourceManager->GetResource< CTexture >("Ezio/CR_U_Ezio_Blason_DiffuseMap.dds");
	ezio = gVars->pResourceManager->GetResource< CMesh >("SK_Mannequin.msh");//   ("Ezio/Ezio.msh");
	meca = gVars->pResourceManager->GetResource< CMesh >("Vanquish/vanquish.msh");
//	city = gVars->pResourceManager->GetResource< CMesh >("castle/castle.msh");
// 
	//shader = gVars->pResourceManager->GetResource< CShaderProgram >("shader.program");
// 	detourshader = gVars->pResourceManager->GetResource< CShaderProgram >("detour.program");
 	whiteshader = gVars->pResourceManager->GetResource< CShaderProgram >("white.program");
// 
	textProgram = gVars->pResourceManager->GetResource< CShaderProgram >("text.program");
	skin = gVars->pResourceManager->GetResource< CShaderProgram >("skinning.program");

	skel = gVars->pResourceManager->GetResource< CSkeleton >("ThirdPersonWalk.skel");

	{
		CMeshHelper m;

		m.AddPosition(Vec3(-100.0f, 0, -100.0f));
		m.AddPosition(Vec3(100.0f, 0, -100.0f));
		m.AddPosition(Vec3(100.0f, 0, 100.0f));
		m.AddPosition(Vec3(-100.0f, 0, 100.0f));
	
		m.AddUV0(Vec2(0, 0));
		m.AddUV0(Vec2(1, 0));
		m.AddUV0(Vec2(1, 1));
		m.AddUV0(Vec2(0, 1));

		m.AddNormal(Vec3(0, 1, 0));
		m.AddNormal(Vec3(0, 1, 0));
		m.AddNormal(Vec3(0, 1, 0));
		m.AddNormal(Vec3(0, 1, 0));

		m.AddMeshPart();
		m.GetMeshPart(0)->AddIndex(0);
		m.GetMeshPart(0)->AddIndex(1);
		m.GetMeshPart(0)->AddIndex(2);

		m.GetMeshPart(0)->AddIndex(2);
		m.GetMeshPart(0)->AddIndex(3);
		m.GetMeshPart(0)->AddIndex(0);

		m.GetMeshPart(0)->AddIndex(0);
		m.GetMeshPart(0)->AddIndex(2);
		m.GetMeshPart(0)->AddIndex(1);

		m.GetMeshPart(0)->AddIndex(3);
		m.GetMeshPart(0)->AddIndex(2);
		m.GetMeshPart(0)->AddIndex(0);
		
		q = m.ConvertToMesh();


		char const ps[] = "\nin lowp vec4 fragmentColor;\n"
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
			"\nuniform mat4 modelMatrix;"
			"\n#else"
			"\nuniform mat4 modelViewMatrix;"
			"\nuniform SceneMatrices"
			"\n{"
			"\nuniform mat4 projectionMatrix;"
			"\n} sm;"
			"\n#endif"
			"\nout vec4 fragmentColor;\n"
			"\nvoid main(void)"
			"\n{"
			"\n#if __GEAR_VR"
			"\ngl_Position = sm.ProjectionMatrix * (sm.ViewMatrix[VIEW_ID] * (modelMatrix * (vec4(inputPosition, 1.0))));"
			"\n#else"
			"\ngl_Position = modelViewMatrix * vec4(inputPosition, 1.0f);"
			"\ngl_Position = sm.projectionMatrix * gl_Position;"
			"\nfragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
			"\n#endif"
			"\n}";

		String err;

		void* psId = gVars->pRenderer->CreateShader(ps, EShaderType::PS);
		void* vsId = gVars->pRenderer->CreateShader(vs, EShaderType::VS);



		pProg = new CShaderProgram();
		pProg->AddShader(new CShader(EShaderType::PS, psId));
		pProg->AddShader(new CShader(EShaderType::VS, vsId));

		pProg->AddAttribute("position");
		pProg->AddAttribute("normal");

		pProg->AddUniformInfo("SceneMatrices", EUniformType::Buffer, sizeof(Matrix44) * 3);
#ifndef __GEAR_VR
		pProg->AddUniformInfo("modelViewMatrix", EUniformType::Matrix4x4, 0);
#else
		pProg->AddUniformInfo("modelMatrix", EUniformType::Matrix4x4, 0);
#endif

		gVars->pRenderer->CompileShader(psId, err);
		gVars->pRenderer->CompileShader(vsId, err);
		pProg->LinkProgram(err);


	}





		CMaterialPtr pMat( new CMaterial() );
		pMat->m_textureLayers[0].m_pTexture = ezioTexture;


		CMeshHelper mh;

		mh.AddPosition(Vec3(0.0f, 500.0f, 0.0f));
		mh.AddPosition(Vec3(20.0f, 500.0f, 0.0f));
		mh.AddPosition(Vec3(20.0f, 500.0f, 20.0f));
		mh.AddPosition(Vec3(0.0f, 500.0f, 20.0f));
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

		font = gVars->pResourceManager->GetResource< CFont >( "font.ttf" );





	
		gVars->pResourceManager->UpdateResourceLoading();

		timer.Start();

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

std::vector<Vec3> positions;

bool first = true;

float a = 0, d = 1;

void* skinMatId;

void CApplication::FrameUpdate()
{
	

	float frameTime = gVars->pOperatingSystem->Tick();
	
	timer.Stop();
	frameTime = timer.GetDuration();
	timer.Start();
	
	float fps = 1.0f / frameTime; (void)fps;

 	CText text( String("Drawcalls : ") + ToString((int)m_pRenderPipeline->GetDrawCalls()) + String("\nPolycount : ") + ToString((int)m_pRenderPipeline->GetPolyCount()) + String("\nFramerate : ") + ToString(fps));
 	textMesh.SetText(text, font);
 
 	CText txt2( U"François Fournel était\ndans la place" );
 	textMesh2.SetText(txt2, font);


	{
		if (first)
		{
			void* data = ezio->GetVertexBuffer()->Lock(true, true);

			size_t vertexCount = ezio->GetVertexBuffer()->GetVertexCount();
			for (size_t i = 0; i < vertexCount; ++i)
			{
				positions.push_back(ezio->GetVertexBuffer()->GetPosition(data, i));
			}



			ezio->GetVertexBuffer()->Unlock();


			skinMatId = gVars->pRenderer->CreateUniformBuffer(sizeof(Matrix44) * 64);

			first = false;
		}


		Transform camInv = m_pRenderPipeline->mainCamera.transform.GetInverse();
		size_t boneCount = skel->GetBones().size();

		Vec3 pos;

		for (size_t i = 0; i < boneCount; ++i)
		{
			int parent = skel->GetBones()[i].GetParentIndex();
			pos = (parent >= 0) ? skel->GetGlobalBindPose().m_boneTransforms[parent].position : skel->GetGlobalBindPose().m_boneTransforms[i].position;

			gVars->pRenderer->DrawLine(camInv * pos, camInv * skel->GetGlobalBindPose().m_boneTransforms[i].position, Color::Green, m_pRenderPipeline->mainCamera.projectionMatrix);
		}


	

		auto itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_03");

		a += frameTime * 100.0f * d;
		if (a > 45.0f)
			d = -1.0f;
		else if (a < -45.0f)
			d = 1.0f;

		CPose pose;
		pose.m_boneTransforms.resize(boneCount);
		pose.m_boneTransforms[*itBoneInfo].rotation = Quat::CreateRotX(Degree(a * 0.25f));

		itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_02");
		pose.m_boneTransforms[*itBoneInfo].rotation = Quat::CreateRotX(Degree(a * 0.25f));

		itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_01");
		pose.m_boneTransforms[*itBoneInfo].rotation = Quat::CreateRotX(Degree(a * 0.25f));

		itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("head");
		pose.m_boneTransforms[*itBoneInfo].rotation = Quat::CreateRotX(Degree(a));





		skel->ConvertFromBindToLocalSpace(pose);
		skel->ComputeGlobalPose(pose, pose);
		skel->ComputeSkinningPose(pose, pose);

		void* data = ezio->GetVertexBuffer()->Lock(false, true);

		size_t vertexCount = ezio->GetVertexBuffer()->GetVertexCount();
		for (size_t i = 0; i < vertexCount; ++i)
		{
			break;
			SVertexBoneWeights& weights = ezio->GetVertexBuffer()->GetBoneWeights(data, i);
			Vec3& position = ezio->GetVertexBuffer()->GetPosition(data, i);

			Vec3 pos = Vec3::Zero;
			for (int j = 0; j < 4; ++j)
			{
				pos = pos + weights.weights[j] * (pose.m_boneTransforms[(int)weights.indices[j]] * positions[i]);
			}

			position = pos;
		}



		ezio->GetVertexBuffer()->Unlock();


		Matrix44* matrices = (Matrix44*)gVars->pRenderer->LockUniformBuffer(skinMatId, 0, 64 * sizeof(Matrix44));
		for(int i = 0; i < Min<int>(boneCount, 64); ++i)
		{
			Matrix34 m;
			m.FromTransform(pose.m_boneTransforms[i]);

			matrices[i] = Matrix44(m);
		}
			
		gVars->pRenderer->UnlockUniformBuffer();



	}





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

	//m_pRenderPipeline->mainCamera.transform.position += m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Backward * 4.0f * frameTime;



	static float aaa = 0;
	aaa += 200.0f * frameTime;
	Transform ttt;
	ttt.rotation = Quat::CreateRotZ(Degree(aaa));// *Quat::CreateRotX(Degree(90.0f));
	ttt.position.z = -5.0f; // 100.0f;
	ttt.position.y = -5.0f;
	ttt.position.x = 7.0f;
	ttt.scale = 5.0f;

	if (pProg)
	{

		for (size_t i = 0; i< meca->GetPartCount();++i)
			CRenderPipeline::AddRenderProxyToQueue(meca.get(), i, m_pRenderPipeline->proxies, ttt, whiteshader.get(), true);

		ttt.rotation = Quat();
		ttt.position.x = 1.0f;
		ttt.scale = Vec3(0.1f, 0.1f, 0.1f);

		Transform t5;
	t5.position.y += 50.0f;

 		for (size_t i = 0; i < ezio->GetPartCount(); ++i)
 			CRenderPipeline::AddRenderProxyToQueue(ezio.get(), i, m_pRenderPipeline->proxies, t5, skin.get(), true);
	}


	Transform textTransf;

	textTransf = m_pRenderPipeline->mainCamera.transform;
	textTransf.position = textTransf.position + m_pRenderPipeline->mainCamera.transform.rotation * Vec3(-750.0f, 1000.0f, 0.0f);

	CRenderPipeline::AddRenderProxyToQueue(textMesh.GetMesh().get(), 0, m_pRenderPipeline->proxies, textTransf, textProgram.get(), true);





	m_pRenderPipeline->Render();




#ifdef __GEAR_VR
	return;
#endif

	return;




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
