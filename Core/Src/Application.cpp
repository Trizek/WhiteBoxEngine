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


class CGizmo
{
public:
	float	ComputeAxisLength( const Transform& invCamTransf, float h, float n ) const
	{
		float scale = (invCamTransf * transform.position).y / n;
		return h * size * scale * 2.0f;
	}

	float	GetRayDistToAxis( int axis, float axisLength, const Vec3& rayOrigin, const Vec3& rayDir, Vec3& projPoint, float& distOnAxis ) const
	{
		Vec3 axes[3] = { Vec3::Right, Vec3::Forward, Vec3::Up };

		Vec3 axisOrigin = m_startPos;
		Vec3 axisDir = transform.TransformVector( axes[axis] );

		Vec3 projOnAxis = ProjectLineOnLine( rayOrigin, rayDir, axisOrigin, axisDir );
		distOnAxis = (projOnAxis - axisOrigin) | axisDir;

		float coeff = Clamp<float>( distOnAxis, 0.0f, axisLength );
		projPoint = axisOrigin + coeff * axes[axis];

		Vec3 projOnRay = ProjectPointOnLine( projPoint, rayOrigin, rayDir );
		return (projOnRay - projPoint).Length();
	}


	int	GetClosestAxisToRay( const Transform& camTransf, float h, float n, const Vec3& rayDir, float& dist, Vec3* pProj = nullptr )
	{
		Transform invCamTransf = camTransf.GetInverse();
		float axisLength = ComputeAxisLength( invCamTransf, h, n );
		Vec3 rayDirNormalized = rayDir; rayDirNormalized.Normalize();

		dist = FLT_MAX;
		Vec3 minProj;
		int minAxis = -1;
		for (int i = 0; i < 3; ++i)
		{
			float distOnAxis;
			Vec3 projPoint;
			float axisDist = GetRayDistToAxis( i, axisLength, camTransf.position, rayDirNormalized, projPoint, distOnAxis );
			if ( axisDist < dist )
			{
				dist = axisDist;
				minAxis = i;
				minProj = projPoint;
			}
		}

		if ( pProj )
		{
			*pProj = minProj;
		}

		return minAxis;
	}

	bool	OnClickPress( const Transform& camTransf, float h, float n, const Vec3& rayDir )
	{
		Transform invCamTransf = camTransf.GetInverse();
		float axisLength = ComputeAxisLength( invCamTransf, h, n );
		Vec3 rayDirNormalized = rayDir; rayDirNormalized.Normalize();

		m_startPos = transform.position;

		Vec3 projPoint;
		float dist;
		int closestAxis = GetClosestAxisToRay( camTransf, h, n, rayDirNormalized, dist, &projPoint );
		if ( dist <= selectDist )
		{
			m_curAxis = closestAxis;
			GetRayDistToAxis( m_curAxis, axisLength, camTransf.position, rayDirNormalized, projPoint, m_startDist);
			m_startPos = transform.position;
			return true;
		}

		return false;
	}

	void	OnClickRelease()
	{
		m_curAxis = -1;
	}

	void	Update( const Transform& camTransf, float h, float n, const Vec3& rayDir, Transform& targetTransform )
	{
		if ( m_curAxis >= 0 )
		{
			Transform invCamTransf = camTransf.GetInverse();
			float axisLength = ComputeAxisLength( invCamTransf, h, n );
			Vec3 rayDirNormalized = rayDir; rayDirNormalized.Normalize();

			Vec3 projPoint;
			float dist;
			float axisValue;
			GetRayDistToAxis( m_curAxis, axisLength, camTransf.position, rayDirNormalized, projPoint, axisValue );

			float absAxisValue = axisValue - m_startDist;

			Vec3 axes[3] = { Vec3::Right, Vec3::Forward, Vec3::Up };

			Vec3 axisDir = transform.TransformVector( axes[m_curAxis] );

			targetTransform.position = m_startPos + axisDir * absAxisValue;
		}
	}


	void	Draw( const Transform& camTransf, float h, float n )
	{
		Transform invCamTransf = camTransf.GetInverse();
		float axisLength = ComputeAxisLength( invCamTransf, h, n );

		Vec3 origin = transform.position;

		gVars->pApplication->m_pRenderPipeline->DrawLine( origin, transform * (Vec3::Right * axisLength), Color::Red );
		gVars->pApplication->m_pRenderPipeline->DrawLine( origin, transform * (Vec3::Forward * axisLength), Color::Green );
		gVars->pApplication->m_pRenderPipeline->DrawLine( origin, transform * (Vec3::Up * axisLength), Color::Blue );
	
	}



public:
	Transform	transform;
	float		size;
	float		selectDist = 25.0f;

	int			m_curAxis = -1;
	Vec3		m_startPos;
	float		m_startDist;
};


CGizmo gizmo;

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
// 
// 	CMeshRenderNode* pEzioMesh = new CMeshRenderNode;
// 	pEzioMesh->AttachTo(pEzioRoot.get());
// 	pEzioMesh->SetLocalTransform(Transform(Vec3(0,0,-75.0f), Quat(), Vec3(100.0f)));
// 	pEzioMesh->SetMesh("Ezio/Ezio.msh");
// 	pEzioMesh->SetShaderProgram(0, "tex.program");

 
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

 	
	gVars->pResourceManager->UpdateResourceLoading();

	gizmo.transform.position.x = 200.0f;




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
 
//	gVars->pPhysicsSystem->SetDebugDraw(true);
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


	if (gVars->pOperatingSystem->IsPressingKey(Key::F2))
	{
		m_pRenderPipeline->mainCamera.transform.position = gizmo.transform.position - m_pRenderPipeline->mainCamera.transform.rotation * Vec3::Forward * 500.0f;
	}


	Degree fov(45.0f);
	float h = Tan(fov * 0.5f) * 10.0f;

	gizmo.size = 0.3f;


	Vec2 mpos = gVars->pOperatingSystem->GetMousePos();
	Vec3 ray = gVars->pApplication->m_pRenderPipeline->mainCamera.GetWorldMousePos(mpos) - m_pRenderPipeline->mainCamera.transform.position;


	Vec3 projp;

	float dist;
	int axis = gizmo.GetClosestAxisToRay(m_pRenderPipeline->mainCamera.transform, h, 10.0f, ray, dist, &projp);

	WbLog("Gizmo", "%d - %.2f", axis, dist);

	m_pRenderPipeline->DrawLine(projp, gVars->pApplication->m_pRenderPipeline->mainCamera.GetWorldMousePos(mpos), Color::Blue);


	bool bClick = gVars->pOperatingSystem->GetMouseButton(0);

	if (bClick)
	{
		if (!click)
		{
			gizmo.OnClickPress(m_pRenderPipeline->mainCamera.transform, h, 10.0f, ray);
		
			
				click = true;
				prevPos = gVars->pOperatingSystem->GetMousePos();
		}
		else
		{

			if (gizmo.m_curAxis == -1)
			{

				Vec2 newPos = gVars->pOperatingSystem->GetMousePos();
				Vec2 delta = newPos - prevPos;

				delta.x *= -200.0f / (float)m_pRenderPipeline->mainCamera.pRenderTarget->GetWidth();
				delta.y *= -200.0f / (float)m_pRenderPipeline->mainCamera.pRenderTarget->GetHeight();

				yaw += delta.x;
				pitch += delta.y;

				m_pRenderPipeline->mainCamera.transform.rotation = Quat(Degree(yaw), Degree(pitch), Degree(0.0f));

				prevPos = newPos;
			}

			
		}
	}
	else
	{
		gizmo.OnClickRelease();
		click = false;
	}
	gizmo.Draw(m_pRenderPipeline->mainCamera.transform, h, 10.0f);
	gizmo.Update(m_pRenderPipeline->mainCamera.transform, h, 10.0f, ray, gizmo.transform);

	pThird->SetGlobalTransform(gizmo.transform);


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
