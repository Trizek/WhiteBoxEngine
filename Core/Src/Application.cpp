#ifndef COOK

#include <math.h>
#include <iostream>

#endif

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
#include "Animation/Animation.h"
#include "Physics/PhysicsSystem.h"

#include "SceneNode.h"
#include "Render/SkinnedMeshRenderNode.h"
#include "Physics/ColliderNode.h"


// #include <iostream>

WHITEBOX_BEGIN



bool click = false;

CApplication::CApplication()
	: m_pRenderPipeline(nullptr){}


CMesh* quad;
CMeshPtr third, ezio;
CMeshPtr meca, city, vanquish;

CTexturePtr ezioTexture;

CShaderProgramPtr shader, detourshader, whiteshader, texprogram, textProgram, skin;

CFontPtr font;
CTextMesh textMesh, textMesh2;


CMeshPtr q;
CShaderProgramPtr pProg;

CTimer timer;

CSkeletonPtr skel;
CAnimationPtr pAnim1, pAnim2;

float blendWeight = 0.5f;
float normalizedTime = 0.0f;


CSceneNode rootNode;
CSceneNode* pNode1, *pNode3;
CSpatialNode* pNode2;
CMeshRenderNode*pNode4;


TRigidBodyHandle sphere;

CSceneNode* pEzioRoot;

CSkinnedMeshRenderNode* pThird;

void CApplication::InitApplication( uint width, uint height )
{
	m_pRenderPipeline = new CRenderPipeline();
	m_pRenderPipeline->Init( width, height );


	m_pRenderPipeline->mainCamera.transform.position = Vec3(0.0f, -600.0f, 150.0f);




#ifndef __GEAR_VR
 	CAssetManager assetManager;
 	assetManager.Export( "../../../Assets", "" );
#endif

	gVars->pResourceManager->ParseResources( "" );


 	skel = gVars->pResourceManager->GetResource< CSkeleton >("ThirdPersonRun.skel");
 
 	pAnim1 = gVars->pResourceManager->GetResource< CAnimation >( "ThirdPersonWalk.anim" );
 	pAnim2 = gVars->pResourceManager->GetResource< CAnimation >( "ThirdPersonRun.anim" );
	

	pEzioRoot = new CSpatialNode;
	pEzioRoot->AttachTo(&rootNode);

	CMeshRenderNode* pEzioMesh = new CMeshRenderNode;
	pEzioMesh->AttachTo(pEzioRoot);
	pEzioMesh->SetLocalTransform(Transform(Vec3(0,0,-75.0f), Quat(), Vec3(100.0f)));
	pEzioMesh->SetMesh("Ezio/Ezio.msh");
	pEzioMesh->SetShaderProgram(0, "tex.program");

 
 	CMeshRenderNode* pHouse = new CMeshRenderNode;
 	pHouse->AttachTo(&rootNode);
 	pHouse->SetLocalTransform(Transform(Vec3(0.0f, 800.0f, 0.0f), Quat(), Vec3(5.0f)));
 	pHouse->SetMesh("Interior/house.msh");
 	pHouse->SetShaderProgram(0, "white.program");

	CStaticMeshColliderNode* pHouseCol = new CStaticMeshColliderNode;
	pHouseCol->SetMesh("Interior/house.msh");
	pHouseCol->AttachTo(pHouse);



	CSphereColliderNode* pSphere = new CSphereColliderNode;
	pSphere->SetMass(1);
	pSphere->SetRadius(50.0f);
	pSphere->AttachTo(&rootNode);
	pSphere->SetLocalTransform(Transform(Vec3(0.0f, 0.0f, 250.0f), Quat()));

	pThird = new CSkinnedMeshRenderNode;
	pThird->AttachTo(pSphere);
	pThird->SetMesh("SK_Mannequin.msh");
	pThird->SetShaderProgram(0, "skinning.program");



 	
	gVars->pResourceManager->UpdateResourceLoading();






		rootNode.Refresh();

// 		Transform houseT;
// 		pHouse->GetGlobalTransform(houseT);
// 		 		TColliderHandle houseShape = gVars->pPhysicsSystem->CreateStaticMeshCollider(CMeshPtr("Interior/house.msh")->GetTriMesh().get(), houseT);// ttt);
// 		 		TRigidBodyHandle houseRB = gVars->pPhysicsSystem->CreateRigidyBody(houseShape, Transform(),0.0f);
// 		 
// 		 			gVars->pPhysicsSystem->AddRigidyBody(houseRB);


		timer.Start();
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

float animTime = 0.0f;

float tim = 0;

void CApplication::FrameUpdate()
{
	

	float frameTime = gVars->pOperatingSystem->Tick();
	
	timer.Stop();
	frameTime = timer.GetDuration();
	timer.Start();
	
	float fps = 1.0f / frameTime; (void)fps;

 	CText text( String("Drawcalls : ") + ToString((int)m_pRenderPipeline->GetDrawCalls()) + String("\nPolycount : ") + ToString((int)m_pRenderPipeline->GetPolyCount()) + String("\nFramerate : ") + ToString(fps));
 //	textMesh.SetText(text, font);
 
	//gVars->pPhysicsSystem->SetDebugDraw(true);
	gVars->pPhysicsSystem->UpdateSimulation(frameTime);
	UpdateColliderProxies();

	



	{
		size_t boneCount = skel->GetBones().size();

	

	

		auto itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_03");


		CPose pose;
		pose.m_boneTransforms.resize(boneCount);
		pose.Zero();


		float factor = 1.f;


	/*	tim = 4.95f;*/
		if (tim < 4.0f)
		{
			blendWeight = 0;
		}
		else if (tim < 6.0f)
		{
			blendWeight = (tim - 4.0f) / 2.0f;
		}
		else if (tim < 10.0f)
		{

			blendWeight = 1.0f;
		}
		else if (tim < 12.0f)
		{
			blendWeight = 1.0f - (tim - 10.0f) / 2.0f;
		}
		else
		{
			tim = 0.0f;
		}
		tim += frameTime * factor;


		/*blendWeight = 1.0f;*/

// 		normalizedTime = 0.60f;

		float length = (1.0f - blendWeight) * pAnim1->m_length + blendWeight * pAnim2->m_length;
		float speed = 1.0f / length;
		normalizedTime += speed * frameTime * factor;
		
		float w = 0.0f;

		pAnim1->AccumulateSamplePose( normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w );
		pAnim2->AccumulateSamplePose( normalizedTime * pAnim2->m_length, blendWeight, pose, w );

		//pAnim1->AccumulateSamplePose(normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w);
		//pAnim2->AccumulateSamplePose(normalizedTime * pAnim2->m_length, blendWeight, pose, w);

		pose.Normalize(w);



		skel->ConvertFromBindToLocalSpace(pose);
		skel->ComputeGlobalPose(pose, pose);

		skel->ComputeSkinningPose(pose, pose);

		pThird->ApplySkinningPose(pose);
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




	m_pRenderPipeline->Render();
}


size_t	CApplication::AddColliderProxy( CSceneNode* pNode, TRigidBodyHandle pRigidBodyHandle )
{
	m_colliderProxies.push_back( { pNode, pRigidBodyHandle } );
	return m_colliderProxies.size() - 1;
}

void	CApplication::RemoveColliderProxy( size_t index )
{
	if ( m_colliderProxies.empty() )
	{
		return;
	}

	size_t newSize = m_colliderProxies.size() - 1;
	if ( index < newSize )
	{
		m_colliderProxies[ index ] = m_colliderProxies[ newSize ];
	}
	m_colliderProxies.resize( newSize );
}

void	CApplication::UpdateColliderProxies()
{
	for ( SColliderProxy& proxy : m_colliderProxies )
	{
		Transform transform;
		gVars->pPhysicsSystem->GetRigidBodyTransform( proxy.pRigidBodyHandle, transform );

		proxy.pNode->SetGlobalTransform( transform );
	}
}

WHITEBOX_END
