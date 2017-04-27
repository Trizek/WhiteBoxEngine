#ifndef COOK

#include <math.h>
#include <iostream>

#endif

#include "Engine.h"
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
#include <algorithm>

#include "SerializableFactory.h"
#include "ScriptSerializer.h"

#include "CommandBuffer.h"


// #include <iostream>

WHITEBOX_BEGIN

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


CSceneNode rootNode("Root");
CSceneNode* pNode1, *pNode3;
CSpatialNode* pNode2;
CMeshRenderNode*pNode4;


TRigidBodyHandle sphere;

CSceneNodePtr pEzioRoot;

CSkinnedMeshRenderNodePtr pThird;
CMeshPtr cube, plane;

std::vector<CSkinnedMeshRenderNodePtr> chars;
std::vector<float> seeds;



CScenePtr pScene;




int charCount = 1;

Vec2 prevPos;
float yaw = 0.0f;
float pitch = 0.0f;

float angle = 0;

float animTime = 0.0f;

float tim = 0;

class CBhv : public IBehavior
{
	DECLARE_SERIALIZABLE_CLASS(CBhv);

	CBhv() : IBehavior()
	{
		int x = 3;
	}

	virtual void Update( float frameTime ) override
	{
		CSpatialNode* pRoot = static_cast< CSpatialNode* >(	GetParentNode() );


		pRoot->Rotate(Degree(160.0f * frameTime), Degree(0), Degree(0));
		
	}
};

DEFINE_SERIALIZABLE_CLASS(CBhv);



class CBhvA : public IBehavior
{
	DECLARE_SERIALIZABLE_CLASS(CBhvA);

	CBhvA() : IBehavior()
	{

	}

	virtual void Update(float frameTime) override
	{


		CSkinnedMeshRenderNode* pNode = static_cast<CSkinnedMeshRenderNode*>(GetParentNode());

		size_t boneCount = skel->GetBones().size();





		auto itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_03");


		CPose pose;
		pose.m_boneTransforms.resize(boneCount);
		pose.Zero();

		{
			float factor = 1.f;



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

			//	pThird->ApplySkinningPose(pose);
		}




		{
			pose.Zero();


			// 		normalizedTime = 0.60f;

			float length = (1.0f - blendWeight) * pAnim1->m_length + blendWeight * pAnim2->m_length;
			float speed = 1.0f / length;

			float w = 0.0f;

			pAnim1->AccumulateSamplePose((normalizedTime) * pAnim1->m_length, 1.0f - blendWeight, pose, w);
			pAnim2->AccumulateSamplePose((normalizedTime) * pAnim2->m_length, blendWeight, pose, w);

			//pAnim1->AccumulateSamplePose(normalizedTime * pAnim1->m_length, 1.0f - blendWeight, pose, w);
			//pAnim2->AccumulateSamplePose(normalizedTime * pAnim2->m_length, blendWeight, pose, w);

			pose.Normalize(w);



			skel->ConvertFromBindToLocalSpace(pose);
			skel->ComputeGlobalPose(pose, pose);

			skel->ComputeSkinningPose(pose, pose);

			pNode->ApplySkinningPose(pose);
		}
		

	}
};

DEFINE_SERIALIZABLE_CLASS(CBhvA);



bool click = false;

CEngine::CEngine()
	: m_pRenderPipeline(nullptr){}


class CCommandLoadResource
{
public:
	void Execute()
	{
		if ( pDescriptor == nullptr )
		{
			return;
		}

		CDataStream dataStream;
		gVars->pOperatingSystem->GetDataStream( pDescriptor->GetPath(), dataStream, pDescriptor->GetSize() );
		IResource* pResource = pDescriptor->GetResourceType()->GetSerializer()->Load( dataStream, *pDescriptor );

		pDescriptor->SetResource( pResource );
	}

	CResourceDescriptor* pDescriptor;
};

void CEngine::InitEngine( uint width, uint height )
{
	m_pRenderPipeline = new CRenderPipeline();
	m_pRenderPipeline->InitPipeline( width, height );

	pMainCamera = new CCamera();
	pMainCamera->transform.position = Vec3(0.0f, -600.0f, 120);
	pMainCamera->pRenderTarget = m_pRenderPipeline->m_pRenderWindow;



	CThreadedCommandBuffer buf;


	buf.PushCommand< CCommandLoadResource >({ nullptr });
	buf.PushCommand< CCommandLoadResource >({ nullptr });

	buf.ExecuteCommands();

	

 	CAssetManager assetManager;
	//assetManager.Export( "../../../Assets", "" );



	gVars->pResourceManager->ParseResources( "" );

	/*
 	
	

	pEzioRoot = new CSpatialNode("Ezio");
	pEzioRoot->AttachTo(&rootNode);
 
 	CMeshRenderNode* pVanquishMesh = new CMeshRenderNode("Vanquish");
 	pVanquishMesh->AttachTo(pEzioRoot.get());
 	pVanquishMesh->SetLocalTransform(Transform(Vec3(-200.0f,500.0f,0.0f), Quat(), Vec3(100.0f)));
	pVanquishMesh->SetMesh("Vanquish/vanquish.msh");
	pVanquishMesh->SetShaderProgram(0, "tex.program");

	CMeshRenderNode* pEzioMesh = new CMeshRenderNode("EzioMesh");
	pEzioMesh->AttachTo(pEzioRoot.get());
	pEzioMesh->SetLocalTransform(Transform(Vec3(0, 500.0f, 0.0f), Quat(), Vec3(100.0f)));
	pEzioMesh->SetMesh("Ezio/Ezio.msh");
	pEzioMesh->SetShaderProgram(0, "tex.program");

	CStaticMeshColliderNode* pEzioCol = new CStaticMeshColliderNode("EzioCol");
	pEzioCol->SetMesh("Ezio/Ezio.msh");// "Interior/house.msh");
	pEzioCol->AttachTo(pEzioMesh);

 
  	CMeshRenderNode* pHouse = new CMeshRenderNode("House");
  	pHouse->AttachTo(&rootNode);
  	pHouse->SetLocalTransform(Transform(Vec3(0.0f, 800.0f, 0.0f), Quat(), Vec3(1.0f)));
 	pHouse->SetMesh("Sponza/sponza.msh");// Interior / house.msh");
 	pHouse->SetShaderProgram(0, "tex.program"); // white.program");
 
 	CStaticMeshColliderNode* pHouseCol = new CStaticMeshColliderNode("HouseCol");
 	pHouseCol->SetMesh("Sponza/sponza.msh");// "Interior/house.msh");
 	pHouseCol->AttachTo(pHouse);



	CSphereColliderNode* pSphere = new CSphereColliderNode("Sphere");
	pSphere->SetMass(1);
	pSphere->SetRadius(50.0f);
	//pSphere->AttachTo(&rootNode);
	pSphere->SetLocalTransform(Transform(Vec3(0.0f, 0.0f, 250.0f), Quat()));



	pThird = new CSkinnedMeshRenderNode("Third");
	//pThird->AttachTo(pSphere);
	pThird->SetMesh("SK_Mannequin.msh");
	pThird->SetShaderProgram(0, "skinning.program");








//	m_curScene = new CScene();


	for (int i = 0; i < charCount; ++i)
	{
	
		CSkinnedMeshRenderNodePtr pNod = new CSkinnedMeshRenderNode("Char");

		pNod->SetLocalTransform(Transform(Vec3(200 + 200 * i, 500.0f, 0.0f), Quat()));

		pNod->AttachTo(&rootNode);
		pNod->SetMesh("SK_Mannequin.msh");
		pNod->SetShaderProgram(0, "skinning.program");

		chars.push_back(pNod);
		seeds.push_back(((float)rand() / (float)RAND_MAX));
	}

	{
		CMeshHelper mh;
		for (int i = 0; i < 6; ++i)
		{
			Transform t;
			float a, b;
			a = i * 90.0f;
			b = (i < 4) ? 0 : ((i == 4) ? -90.0f : 90.0f);
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
	}

	{
		CMeshHelper mh;

		Vec3 n(0.0f, 0.0f, 1.0f);

		mh.AddPosition(Vec3(-1.0f, -1.0f, 0.0f)); mh.AddNormal(n);
		mh.AddPosition(Vec3(1.0f, -1.0f, 0.0f)); mh.AddNormal(n);
		mh.AddPosition(Vec3(1.0f, 1.0f, 0.0f)); mh.AddNormal(n);
		mh.AddPosition(Vec3(-1.0f, 1.0f, 0.0f)); mh.AddNormal(n);

		mh.AddMeshPart();
		mh.GetMeshPart(0)->AddTriangle(0, 1, 2);
		mh.GetMeshPart(0)->AddTriangle(2, 3, 0);

		plane = mh.ConvertToMesh();
	}
 


	Transform t;
	t.position = Vec3(0.0f, -950.0f, 10.0f);
	//t.scale = Vec3(100, 100, 100);
	
		CBoxColliderNode* pBox1 = new CBoxColliderNode("Box");
		{
		pBox1->SetSize(Vec3(100.0f));


		pBox1->AttachTo(&rootNode);
		pBox1->SetMass(2.0f);

		CMeshRenderNode* pRenderBox = new CMeshRenderNode("Box");
		pRenderBox->AttachTo(pBox1);
		pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(100, 100, 100)));
		pRenderBox->SetMesh(cube);
		pRenderBox->SetShaderProgram(0, "white.program");

		pBox1->SetLocalTransform(t);

		//gVars->pPhysicsSystem->SetDebugDraw( true );
	}

		CBoxColliderNode* pBox2 = new CBoxColliderNode("Box");
		{
			pBox2->SetSize(Vec3(100.0f));


			pBox2->AttachTo(&rootNode);
			pBox2->SetMass(2.0f);

			CMeshRenderNode* pRenderBox = new CMeshRenderNode("Box2");
			pRenderBox->AttachTo(pBox2);
			pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(100,100,100)));
			pRenderBox->SetMesh(cube);
			pRenderBox->SetShaderProgram(0, "white.program");

			pBox2->SetLocalTransform(t);

		}

 	SpawnCube(Transform());*/


	//CScriptNodeWriter wn;

	//CSceneNode* pRoot = &rootNode;
	//CSerializableFactory::Get().Serialize(pRoot, wn);


	//CScriptFileWriter writer("Scene.txt");
	//writer.WriteNode(wn.GetRootNode());


//
//CDataStream dataStream;
//gVars->pOperatingSystem->GetDataStream("Scene.txt", dataStream);
//
//
//CScriptStreamReader streamReader;
//CScriptNodeReader reader( streamReader.Parse(dataStream) );
//
//
//
//CSceneNode* pRoot = nullptr;
//
//CSerializableFactory::Get().Serialize(pRoot, reader);


//pRoot->AttachTo(&rootNode);

// 
// 

skel = "ThirdPersonRun.skel";

pAnim1 = "ThirdPersonWalk.anim";
pAnim2 = "ThirdPersonRun.anim";

pScene = "Main.scene";
	gVars->pResourceManager->UpdateResourceLoading();


	pScene->Refresh();

	//	rootNode.Refresh();

		timer.Start();



}

void CEngine::Resize( uint width, uint height )
{


	pMainCamera->pRenderTarget->SetWidth( width );
	pMainCamera->pRenderTarget->SetHeight( height );
}



void CEngine::FrameUpdate()
{

	float frameTime = gVars->pOperatingSystem->Tick();
	
	timer.Stop();
	frameTime = timer.GetDuration();
	timer.Start();
	
	float fps = 1.0f / frameTime; (void)fps;



//	gVars->pPhysicsSystem->SetDebugDraw(true);

	gVars->pBehaviorManager->Update( frameTime );
	gVars->pPhysicsSystem->UpdateSimulation( frameTime );
	UpdateColliderProxies();

	
/*


	{
		size_t boneCount = skel->GetBones().size();





		auto itBoneInfo = skel->GetBoneNameToIndexMap().FindElement("spine_03");


		CPose pose;
		pose.m_boneTransforms.resize(boneCount);
		pose.Zero();

		{
		float factor = 1.f;



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

	//	pThird->ApplySkinningPose(pose);
	}

		

		for (int i = 0; i < chars.size(); ++i)
		{
			pose.Zero();


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
	*/
	pMainCamera->transform.rotation = Quat::CreateRotZ(Degree(-45.0f));


	if ( gVars->pOperatingSystem->IsPressingKey( Key::D ) )
	{
		pMainCamera->transform.position += pMainCamera->transform.rotation * Vec3::Right * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::Q ) )
	{
		pMainCamera->transform.position += pMainCamera->transform.rotation * Vec3::Left * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::Z ) )
	{
		pMainCamera->transform.position += pMainCamera->transform.rotation * Vec3::Forward * 500.0f * frameTime;
	}
	if ( gVars->pOperatingSystem->IsPressingKey( Key::S ) )
	{
		pMainCamera->transform.position += pMainCamera->transform.rotation * Vec3::Backward * 500.0f * frameTime;
	}
// 
// 	bool bMovingCamera = gVars->pOperatingSystem->GetMouseButton(0);
// 	if (m_bMovingCamera != bMovingCamera)
// 	{
// 		//m_prevMousePos = gVars->pOperatingSystem->GetMousePos();
// 	}
// 	if ( m_bMovingCamera )
// 	{
// 		Vec2 mousePos = gVars->pOperatingSystem->GetMousePos();
// 		pMainCamera->transform.rotation.
// 	}

/*

	bool bSpawnCube = gVars->pOperatingSystem->JustPressedKey(Key::F1);
	
	STouchEvent touchEvent;
	while (gVars->pOperatingSystem->PollTouchEvent(touchEvent))
	{
		bSpawnCube = bSpawnCube || (touchEvent.eventType == ETouchEventType::End);
	}

	if (bSpawnCube)
	{

		CBoxColliderNode* pNode = SpawnCube(pMainCamera->transform * Transform(Vec3::Forward * 50.0f, Quat(), Vec3(40.0f)));
		pNode->Refresh();
		pNode->AddImpulse(pMainCamera->transform.rotation * Vec3::Forward * 100.0f);
	}
*/

	pMainCamera->ComputeProjectionMatrix();
	pMainCamera->ComputeInverseTransformMatrix();
	m_pRenderPipeline->SetCamera(*pMainCamera.get());
	m_pRenderPipeline->Render();
}


size_t	CEngine::AddColliderProxy( CSceneNode* pNode, TRigidBodyHandle pRigidBodyHandle )
{
	m_colliderProxies.push_back( { pNode, pRigidBodyHandle } );
	return m_colliderProxies.size() - 1;
}

void	CEngine::RemoveColliderProxy( size_t index )
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

void	CEngine::UpdateColliderProxies()
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
