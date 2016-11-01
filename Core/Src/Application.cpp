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
#include "BehaviorManager.h"
#include "Behavior.h"



// #include <iostream>

WHITEBOX_BEGIN


class CBhv : public IBehavior
{
	virtual void Update( float frameTime ) override
	{
		CSpatialNode* pRoot = static_cast< CSpatialNode* >(	GetParentNode() );
		
	}
};

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

CSceneNodePtr pEzioRoot;

CSkinnedMeshRenderNodePtr pThird;
CMeshPtr cube;

std::vector<CSkinnedMeshRenderNodePtr> chars;
std::vector<float> seeds;

CBoxColliderNode*	SpawnCube(const Transform& transform)
{
	CBoxColliderNode* pBox = new CBoxColliderNode;
	
	pBox->SetSize(Vec3(1.0f));


	pBox->AttachTo(&rootNode);
	pBox->SetMass(2.0f);

	CMeshRenderNode* pRenderBox = new CMeshRenderNode;
	pRenderBox->AttachTo(pBox);
	pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat()));
	pRenderBox->SetMesh(cube);
	pRenderBox->SetShaderProgram(0, "white.program");

	pBox->SetLocalTransform(transform);

	return pBox;
}


int charCount = 1;

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


 	skel = "ThirdPersonRun.skel";
 
 	pAnim1 = "ThirdPersonWalk.anim";
 	pAnim2 = "ThirdPersonRun.anim";
	

	pEzioRoot = new CSpatialNode;
	pEzioRoot->AttachTo(&rootNode);
 
 	CMeshRenderNode* pVanquishMesh = new CMeshRenderNode;
 	pVanquishMesh->AttachTo(pEzioRoot.get());
 	pVanquishMesh->SetLocalTransform(Transform(Vec3(-200.0f,500.0f,0.0f), Quat(), Vec3(1000.0f)));
	pVanquishMesh->SetMesh("Determinism/GECKO/GECKO.msh"); // Vanquish / vanquish.msh");
	pVanquishMesh->SetShaderProgram(0, "white.program");// tex.program");

	CMeshRenderNode* pEzioMesh = new CMeshRenderNode;
	pEzioMesh->AttachTo(pEzioRoot.get());
	pEzioMesh->SetLocalTransform(Transform(Vec3(0, 500.0f, 0.0f), Quat(), Vec3(100.0f)));
	pEzioMesh->SetMesh("Ezio/Ezio.msh");
	pEzioMesh->SetShaderProgram(0, "tex.program");

	CStaticMeshColliderNode* pEzioCol = new CStaticMeshColliderNode;
	pEzioCol->SetMesh("Ezio/Ezio.msh");// "Interior/house.msh");
	pEzioCol->AttachTo(pEzioMesh);

 
 	CMeshRenderNode* pHouse = new CMeshRenderNode;
 	pHouse->AttachTo(&rootNode);
 	pHouse->SetLocalTransform(Transform(Vec3(0.0f, 800.0f, 0.0f), Quat(), Vec3(1.0f)));
	pHouse->SetMesh("Sponza/sponza.msh");// Interior / house.msh");
	pHouse->SetShaderProgram(0, "tex.program"); // white.program");

	CStaticMeshColliderNode* pHouseCol = new CStaticMeshColliderNode;
	pHouseCol->SetMesh("Sponza/sponza.msh");// "Interior/house.msh");
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

	for (int i = 0; i < charCount; ++i)
	{
		CSkinnedMeshRenderNodePtr pNod = new CSkinnedMeshRenderNode;

		pNod->SetLocalTransform(Transform(Vec3(200 + 200 * i, 500.0f, 0.0f), Quat()));

		pNod->AttachTo(&rootNode);
		pNod->SetMesh("SK_Mannequin.msh");
		pNod->SetShaderProgram(0, "skinning.program");

		chars.push_back(pNod);
		seeds.push_back(((float)rand() / (float)RAND_MAX));
	}

	
	CMeshHelper mh;
	for (int i = 0; i < 6; ++i)
	{
		Transform t;
		float a, b;
		a = i * 90.0f;
		b = (i < 4) ? 0 : ((i == 4)? -90.0f : 90.0f);
		t.rotation = Quat(Degree(a), Degree(b), Degree(0));


		Vec3 p0 = t * Vec3(-0.5f, 0.5f, 0.5f);
		Vec3 p1 = t * Vec3(0.5f, 0.5f, 0.5f);
		Vec3 p2 = t * Vec3(0.5f, 0.5f, -0.5f);
		Vec3 p3 = t * Vec3(-0.5f, 0.5f, -0.5f);
		Vec3 n = t.rotation * Vec3(0, 1, 0);


		mh.AddPosition(p0); mh.AddNormal(n);
		mh.AddPosition(p1); mh.AddNormal(n);
		mh.AddPosition(p2); mh.AddNormal(n);
		mh.AddPosition(p3); mh.AddNormal(n);
		
		int iTri = 4 * i;
		mh.AddMeshPart();
		mh.GetMeshPart(i)->AddTriangle(iTri, iTri + 1, iTri + 2);
		mh.GetMeshPart(i)->AddTriangle(iTri + 2, iTri + 3, iTri);
	}
	cube = mh.ConvertToMesh();

	Vec3 origin(150.0f, -200.0f, 350.0f);
	float size = 40.0f;
	for (int i = 0; i < 5; ++i)
	{
		break;
		for (int j = 0; j < 5; ++j)
		{
			Vec3 pos = origin + Vec3(i * size * 2.0f, 0.0f, j * size * 2.0f);

			SpawnCube(Transform(pos, Quat(), Vec3(size)));

			continue;


			CBoxColliderNodePtr pBox = new CBoxColliderNode;
			pBox->AttachTo(&rootNode);
			pBox->SetSize(Vec3(size));

			
			pBox->AttachTo(&rootNode);
			pBox->SetLocalTransform(Transform(pos, Quat()));
			pBox->SetMass(2.0f);

			CMeshRenderNodePtr pRenderBox = new CMeshRenderNode;
			pRenderBox->AttachTo(pBox.get());
			pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(size)));
			pRenderBox->SetMesh(cube);
			pRenderBox->SetShaderProgram(0, "white.program");
		}
	}

	SpawnCube(Transform());

 	
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

	if (((int)(frameTime * 1000)) % 1000 == 0)
		printf("fps %.2f\n", fps);

 	CText text( String("Drawcalls : ") + ToString((int)m_pRenderPipeline->GetDrawCalls()) + String("\nPolycount : ") + ToString((int)m_pRenderPipeline->GetPolyCount()) + String("\nFramerate : ") + ToString(fps));
 //	textMesh.SetText(text, font);
 
//	gVars->pPhysicsSystem->SetDebugDraw(true);

	gVars->pBehaviorManager->Update( frameTime );
	gVars->pPhysicsSystem->UpdateSimulation( frameTime );
	UpdateColliderProxies();

	



	{
		size_t boneCount = skel->GetBones().size();





		auto itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_03");


		CPose pose;
		pose.m_boneTransforms.resize(boneCount);
		pose.Zero();

		{
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

		pAnim1->AccumulateSamplePose(normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w);
		pAnim2->AccumulateSamplePose(normalizedTime * pAnim2->m_length, blendWeight, pose, w);

		//pAnim1->AccumulateSamplePose(normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w);
		//pAnim2->AccumulateSamplePose(normalizedTime * pAnim2->m_length, blendWeight, pose, w);

		pose.Normalize(w);



		skel->ConvertFromBindToLocalSpace(pose);
		skel->ComputeGlobalPose(pose, pose);

		skel->ComputeSkinningPose(pose, pose);

		pThird->ApplySkinningPose(pose);
	}

		

		for (int i = 0; i < chars.size(); ++i)
		{
			pose.Zero();
			/*blendWeight = 1.0f;*/

			// 		normalizedTime = 0.60f;

			float length = (1.0f - blendWeight) * pAnim1->m_length + blendWeight * pAnim2->m_length;
			float speed = 1.0f / length;

			float w = 0.0f;

			pAnim1->AccumulateSamplePose((normalizedTime + seeds[i]) * pAnim1->m_length, 1.0f - blendWeight, pose, w);
			pAnim2->AccumulateSamplePose((normalizedTime + seeds[i]) * pAnim2->m_length, blendWeight, pose, w);

			//pAnim1->AccumulateSamplePose(normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w);
			//pAnim2->AccumulateSamplePose(normalizedTime * pAnim2->m_length, blendWeight, pose, w);

			pose.Normalize(w);



			skel->ConvertFromBindToLocalSpace(pose);
			skel->ComputeGlobalPose(pose, pose);

			skel->ComputeSkinningPose(pose, pose);

			chars[i]->ApplySkinningPose(pose);
		}
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

	bool bSpawnCube = gVars->pOperatingSystem->JustPressedKey(Key::F1);
	
	STouchEvent touchEvent;
	while (gVars->pOperatingSystem->PollTouchEvent(touchEvent))
	{
		bSpawnCube = bSpawnCube || (touchEvent.eventType == ETouchEventType::End);
	}

	if (bSpawnCube)
	{

		CBoxColliderNode* pNode = SpawnCube(m_pRenderPipeline->mainCamera.transform * Transform(Vec3::Forward * 50.0f, Quat(), Vec3(40.0f)));
		pNode->Refresh();
		pNode->AddImpulse(m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Forward * 100.0f);
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
		
		Transform prevTransform;
		proxy.pNode->GetGlobalTransform(prevTransform);
		transform.scale = prevTransform.scale;

		proxy.pNode->SetGlobalTransform( transform );
	}
}

WHITEBOX_END
