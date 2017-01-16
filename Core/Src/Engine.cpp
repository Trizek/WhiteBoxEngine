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



// #include <iostream>

WHITEBOX_BEGIN


struct Synapse
{
	int from;
	float weight;
};

struct Neuron
{
	std::vector<Synapse>	inputs;
};

struct NeuralNetwork
{
	std::vector<Neuron>		neurons;
	std::vector<float>		neuronsVals;
	std::vector<bool>		nodeEvaluated;

	int		inputCount, outputCount;

	void	Fit( size_t size )
	{
		if ( size > neurons.size() )
		{
			neurons.resize(size);
			neuronsVals.resize(size);
			nodeEvaluated.resize(size);
		}
	}

	void	Clear()
	{
		memset( &neuronsVals[0], 0, sizeof(float) * neuronsVals.size() );
		memset( &nodeEvaluated[0], 0, sizeof(bool) * nodeEvaluated.size() );
	}


	void	EvaluateNode( int node )
	{
		if ( node <= inputCount || nodeEvaluated[node] )
		{
			return;
		}

		float val = 0.0f;

		const Neuron& neuron = neurons[node];
		for( const Synapse& syn : neuron.inputs )
		{
			EvaluateNode( syn.from );
			val += neuronsVals[syn.from] * syn.weight;
		}


		neuronsVals[node] = ActivationFunction( val );
		nodeEvaluated[node] = true;
	}


	void	Evaluate()
	{
		for ( size_t i = inputCount; i < neurons.size(); ++i )
		{
			EvaluateNode(i);
		}
	}

	static float ActivationFunction( float val )
	{
		return (val > 0.0f) ? 1.0f : 0.0f;
	}
};

struct NeuralGene
{
	int			innovationNumber;
	float		weight;
	short int	from, to;
	bool		disabled;
};

struct NeuralGenotype
{
	int inputCount, outputCount, neuronCount;
	std::vector<NeuralGene>	genes;
	std::vector<int>	neuronLayers;

	NeuralGenotype() = default;

	NeuralGenotype( int _intputCount, int _outputCount )
		: inputCount(_intputCount), outputCount(_outputCount), neuronCount(_intputCount + _outputCount)
	{
		neuronLayers.resize(neuronCount);
		for (int i = 0; i < inputCount; ++i)
		{
			neuronLayers[i] = 0;
		}
		for (int i = inputCount; i < inputCount + outputCount; ++i)
		{
			neuronLayers[i] = 1;
		}
	}

	void CreateNeuralNetwork( NeuralNetwork& network ) const
	{
		network.Fit( neuronCount );

		network.inputCount = inputCount;
		network.outputCount = outputCount;

		for( const NeuralGene& gene : genes )
		{
			network.neurons[ gene.to ].inputs.push_back( { gene.from, gene.weight} );
		}

		network.Clear();
	}

	int GetEnabledGeneCount() const
	{
		int enabledGenes = 0;
		for( const NeuralGene& gene : genes )
		{
			if (!gene.disabled)
				++enabledGenes;
		}
		return enabledGenes;
	}

	int	GetEnabledGene( int i ) const
	{
		for( int j = 0; j < genes.size(); ++j )
		{
			if ( genes[j].disabled )
				continue;

			if ( i == 0 )
			{
				return j;
			}
			--i;
		}
	}

	void MutateWeight( int mutateGene )
	{
		NeuralGene& gene = genes[ mutateGene ];
		float mutationBound = Max<float>( fabsf(gene.weight) * 0.5f, 0.25f );
		gene.weight += RandomFloat(-mutationBound, mutationBound);
	}

	void MutateAddNode()
	{
		int enabledGenes = GetEnabledGeneCount();
		if ( enabledGenes == 0 )
		{
			return;
		}

		int randomGene = RandomInt( 0, enabledGenes - 1 );
		randomGene = GetEnabledGene( randomGene );

		NeuralGene& gene = genes[ randomGene ];
		gene.disabled = true;

	
		short int newNode = (short int)(neuronCount++);
		int innovationNumber = genes.back().innovationNumber + 1;
		genes.push_back( {innovationNumber, 0.0f, gene.from, newNode, false} );
		genes.push_back( {innovationNumber + 1, gene.weight, newNode, gene.to, false} );

		neuronLayers.push_back(neuronLayers[gene.from] + 1);

		IncreaseLayer(gene.to);
	}

	void IncreaseLayer(int node)
	{
		neuronLayers[node]++;
		for( const NeuralGene& gene : genes )
		{
			if ( gene.from == node )
			{
				IncreaseLayer(gene.to);
			}
		}
	}

	struct SLink
	{
		int link;

		SLink() = default;
		SLink( int _link ) : link(link){}
		SLink( int from, int to, int nodeCount ) : link(from * nodeCount + to){}

		SLink& operator=( int _link )
		{
			link = _link;
			return *this;
		}

		int	From( int nodeCount ) const
		{
			return link / nodeCount;
		}

		int	To( int nodeCount ) const
		{
			return link % nodeCount;
		}

		operator int() const
		{
			return link;
		}
	};

	void MutateAddConnection()
	{
		std::vector<SLink> links;
		links.resize(neuronCount * neuronCount);
		for (int i = 0; i < links.size(); ++i)
		{
			links[i] = i;
		}

		for (int from = 0; from < neuronCount; ++from)
		{
			for (int to = 0; to < neuronCount; ++to)
			{
				if (neuronLayers[to] <= neuronLayers[from])
				{
					links[SLink(from, to, neuronCount)] = -1;
				}
			}
		}

		for ( const NeuralGene& gene : genes )
		{
			links[SLink(gene.from, gene.to, neuronCount)] = -1;
		}

		links.erase( std::remove(links.begin(), links.end(), -1), links.end() );

// 		for ( const NeuralGene& gene : genes )
// 		{
// 			if ( FindPath(gene.from, gene.to) )
// 				links[SLink(gene.from, gene.to, neuronCount)] = -1;
// 		}
// 
// 		links.erase(std::remove(links.begin(), links.end(), -1), links.end());

		if ( links.empty() )
			return;

		int randomLink = RandomInt( 0, links.size() - 1 );
		SLink newLink = links[randomLink];

		int innovationNumber = (genes.empty())? 0 : genes.back().innovationNumber + 1;
		genes.push_back( {innovationNumber, 0.0f, (short int)newLink.From(neuronCount), (short int)newLink.To(neuronCount), false} );
	}

	bool FindPath( int from, int to ) const
	{
		for( const NeuralGene& gene : genes )
		{
			if ( neuronLayers[gene.from] > neuronLayers[from] )
				return false;

			if ( gene.from == from && FindPath(gene.to, to) )
			{
				return true;
			}
		}
		return false;
	}

	void Mutate( float weightProbability, float connectionProbability, float nodeProbability )
	{
		if ( RandomProbability(connectionProbability) )
		{
			MutateAddConnection();
		}
		if ( RandomProbability(nodeProbability) )
		{
			MutateAddNode();
		}
		for( int gene = 0; gene < genes.size(); ++gene )
		{
			if ( RandomProbability(weightProbability) )
			{
				MutateWeight(gene);
			}
		}
	}
};




struct SLimb
{
	Vec3	size;
};

struct SAnchor
{
	Vec3	location;
	Vec3	axis;
};

struct SJoint
{
	int			fromIdx;
	SAnchor		fromAnchor;

	int			toIdx;
	SAnchor		toAnchor;
};

struct SBody
{
	TRigidBodyHandle	rigidBody;
	Vec3				size;
};



struct SGenotype
{
public:
	std::vector<SLimb>			limbs;
	std::vector<SJoint>			joints;

	NeuralGenotype				brainGenotype;
};



struct SPhenotype
{
	std::vector<TRigidBodyHandle>	bodies;
	NeuralNetwork					brainPh;

	const SGenotype*				pGenotype;

	void							ApplyBodyTorque(int idx, float intensity, const SAnchor& anchor)
	{
		Transform transform;
		const SLimb& body = pGenotype->limbs[idx];

		gVars->pPhysicsSystem->GetRigidBodyTransform(bodies[idx], transform);
		Vec3 globalAxis = transform.rotation * anchor.axis;
		Vec3 globalPoint = transform.rotation * (anchor.location * body.size);

		Vec3 torque = globalPoint ^ globalAxis;

		gVars->pPhysicsSystem->AddTorque(bodies[idx], globalAxis * intensity * 500.0f);
	}

	void							ApplyLinkForce(int idx, float intensity)
	{
		const SJoint& joint = pGenotype->joints[idx];
		ApplyBodyTorque(joint.fromIdx, intensity, joint.fromAnchor);
		ApplyBodyTorque(joint.toIdx, -intensity, joint.toAnchor);
	}

	float							GetJointAngle( int idx ) const
	{
		const SJoint& joint = pGenotype->joints[ idx ];
		Transform t1, t2;
		gVars->pPhysicsSystem->GetRigidBodyTransform( bodies[joint.fromIdx], t1 );
		gVars->pPhysicsSystem->GetRigidBodyTransform( bodies[joint.toIdx], t2 );
		Vec3 axis1 = t1.rotation * joint.fromAnchor.axis;
		Vec3 point1 = t1 * (joint.fromAnchor.location * pGenotype->limbs[joint.fromIdx].size);
		Vec3 axis2 = t2.rotation * joint.toAnchor.axis;
		Vec3 point2 = t2 * (joint.toAnchor.location * pGenotype->limbs[joint.toIdx].size);
		
		Vec3 v1 = t1.position - point1;
		v1 = v1 - axis1 * (v1 | axis1);
		v1.Normalize();

		Vec3 v2 = t2.position - point2;
		v2 = v2 - axis2 * (v2 | axis2);
		v2.Normalize();

		return v1 | v2;
	}



	void		Update( float time )
	{
// 		MarkBrainNodesNotEvaluated();
// 		RetrieveNeuralInputs();
// 		brainPh.Evaluate( pGenotype->brainGenotype, time );
// 
// 		int startIndex = (int)brainPh.nodesValues.size() - pGenotype->brainGenotype.outputNodesCount;
// 		for (int i = 0; i < pGenotype->brainGenotype.outputNodesCount; ++i)
// 		{
// 			float outputVal = brainPh.nodesValues[i];
// 			ApplyLinkForce(i, Clamp(outputVal, 0.0f, 1.0f));
// 		}
	}
};

void BuildPhenotype( const SGenotype& gen, SPhenotype& ph, const Vec3& location )
{
	ph.pGenotype = &gen;

	ph.bodies.reserve(gen.limbs.size());

	for( const SLimb& limb : gen.limbs )
	{
		TColliderHandle col = gVars->pPhysicsSystem->CreateBoxCollider( limb.size );
		TRigidBodyHandle rb = gVars->pPhysicsSystem->CreateRigidyBody( col, Transform(location, Quat()), 2.0f );

		gVars->pPhysicsSystem->AddRigidyBody(rb);

		ph.bodies.push_back( rb );
	}

	for( const SJoint& transition : gen.joints )
	{
		Vec3 ptA( transition.fromAnchor.location.x * gen.limbs[transition.fromIdx].size.x, transition.fromAnchor.location.y * gen.limbs[transition.fromIdx].size.y, transition.fromAnchor.location.z * gen.limbs[transition.fromIdx].size.z );
		Vec3 ptB( transition.toAnchor.location.x * gen.limbs[transition.toIdx].size.x, transition.toAnchor.location.y * gen.limbs[transition.toIdx].size.y, transition.toAnchor.location.z * gen.limbs[transition.toIdx].size.z );
		gVars->pPhysicsSystem->AddHingeConstraint( ph.bodies[transition.fromIdx], ph.bodies[transition.toIdx], ptA * 0.5f, ptB * 0.5f, transition.fromAnchor.axis, transition.toAnchor.axis, 0, 1 );
	}

	gen.brainGenotype.CreateNeuralNetwork(ph.brainPh);
}

void CreateGenotype(SGenotype& gen)
{
	gen.limbs.resize(8);
	gen.joints.resize(7);

	gen.limbs[0] = { Vec3(100, 100, 20) };
	gen.limbs[1] = { Vec3(100, 100, 20) };
	gen.limbs[2] = { Vec3(100, 100, 20) };
	gen.limbs[4] = { Vec3(100, 100, 20) };
	gen.limbs[5] = { Vec3(100, 100, 20) };
	gen.limbs[6] = { Vec3(100, 100, 20) };
	gen.limbs[7] = { Vec3(100, 100, 20) };

	gen.joints[0] = { 0, {Vec3(1,0,0), Vec3(0,1,0)}, 1,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[1] = { 1,{ Vec3(1,0,0), Vec3(0,1,0) }, 2,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[2] = { 2,{ Vec3(1,0,0), Vec3(0,1,0) }, 3,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[3] = { 3,{ Vec3(1,0,0), Vec3(0,1,0) }, 4,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[4] = { 4,{ Vec3(1,0,0), Vec3(0,1,0) }, 5,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[5] = { 5,{ Vec3(1,0,0), Vec3(0,1,0) }, 6,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	gen.joints[6] = { 6,{ Vec3(1,0,0), Vec3(0,1,0) }, 7,{ Vec3(-1,0,0), Vec3(0,1,0) } };
	//gen.transitions[7] = { 7,{ Vec3(1,0,0), Vec3(0,1,0) }, 0,{ Vec3(-1,0,0), Vec3(0,1,0) } };

	//gen.brainGenotype.CreateRandomGenotype((int)gen.limbs.size());

	NeuralGenotype g(4, 1);

	NeuralGenotype h = g, f = g, i = g;

	g.Mutate(0.5f, 0.85f, 0.5f);
	h.Mutate(0.5f, 0.85f, 0.5f);
	i.Mutate(0.5f, 0.85f, 0.5f);
	f.Mutate(0.5f, 0.85f, 0.5f);

	int x = 2;
}


class CBhv : public IBehavior
{
	virtual void Update( float frameTime ) override
	{
		CSpatialNode* pRoot = static_cast< CSpatialNode* >(	GetParentNode() );
		
	}
};

bool click = false;

CEngine::CEngine()
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
CMeshPtr cube, plane;

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

SPhenotype ph;

void CEngine::InitEngine( uint width, uint height )
{
	m_pRenderPipeline = new CRenderPipeline();
	m_pRenderPipeline->Init( width, height );


	m_pRenderPipeline->mainCamera.transform.position = Vec3(0.0f, -600.0f, 120);// 150.0f);




#ifndef __GEAR_VR
 	CAssetManager assetManager;
 	assetManager.Export( "../../../Assets", "" );
#endif

	gVars->pResourceManager->ParseResources( "" );


 	skel = "ThirdPersonRun.skel";
 
 	pAnim1 = "ThirdPersonWalk.anim";
 	pAnim2 = "ThirdPersonRun.anim";
	

	//pEzioRoot = new CSpatialNode;
	//pEzioRoot->AttachTo(&rootNode);
 //
 //	CMeshRenderNode* pVanquishMesh = new CMeshRenderNode;
 //	//pVanquishMesh->AttachTo(pEzioRoot.get());
 //	pVanquishMesh->SetLocalTransform(Transform(Vec3(-200.0f,500.0f,0.0f), Quat(), Vec3(1000.0f)));
	//pVanquishMesh->SetMesh("Determinism/GECKO/GECKO.msh"); // Vanquish / vanquish.msh");
	//pVanquishMesh->SetShaderProgram(0, "white.program");// tex.program");

	//CMeshRenderNode* pEzioMesh = new CMeshRenderNode;
	////pEzioMesh->AttachTo(pEzioRoot.get());
	//pEzioMesh->SetLocalTransform(Transform(Vec3(0, 500.0f, 0.0f), Quat(), Vec3(100.0f)));
	//pEzioMesh->SetMesh("Ezio/Ezio.msh");
	//pEzioMesh->SetShaderProgram(0, "tex.program");

	//CStaticMeshColliderNode* pEzioCol = new CStaticMeshColliderNode;
	//pEzioCol->SetMesh("Ezio/Ezio.msh");// "Interior/house.msh");
	//pEzioCol->AttachTo(pEzioMesh);

 
//  	CMeshRenderNode* pHouse = new CMeshRenderNode;
//  	pHouse->AttachTo(&rootNode);
//  	pHouse->SetLocalTransform(Transform(Vec3(0.0f, 800.0f, 0.0f), Quat(), Vec3(1.0f)));
// 	pHouse->SetMesh("Sponza/sponza.msh");// Interior / house.msh");
// 	pHouse->SetShaderProgram(0, "tex.program"); // white.program");
// 
// 	CStaticMeshColliderNode* pHouseCol = new CStaticMeshColliderNode;
// 	pHouseCol->SetMesh("Sponza/sponza.msh");// "Interior/house.msh");
// 	pHouseCol->AttachTo(pHouse);



	//CSphereColliderNode* pSphere = new CSphereColliderNode;
	//pSphere->SetMass(1);
	//pSphere->SetRadius(50.0f);
	////pSphere->AttachTo(&rootNode);
	//pSphere->SetLocalTransform(Transform(Vec3(0.0f, 0.0f, 250.0f), Quat()));



	//pThird = new CSkinnedMeshRenderNode;
	////pThird->AttachTo(pSphere);
	//pThird->SetMesh("SK_Mannequin.msh");
	//pThird->SetShaderProgram(0, "skinning.program");


	m_curScene = new CScene();


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
 
// 	Vec3 origin(150.0f, -200.0f, 350.0f);
// 	float size = 40.0f;
// 	for (int i = 0; i < 5; ++i)
// 	{
// 		break;
// 		for (int j = 0; j < 5; ++j)
// 		{
// 			Vec3 pos = origin + Vec3(i * size * 2.0f, 0.0f, j * size * 2.0f);
// 
// 			SpawnCube(Transform(pos, Quat(), Vec3(size)));
// 
// 			continue;
// 
// 
// 			CBoxColliderNodePtr pBox = new CBoxColliderNode;
// 			pBox->AttachTo(&rootNode);
// 			pBox->SetSize(Vec3(size));
// 
// 			
// 			pBox->AttachTo(&rootNode);
// 			pBox->SetLocalTransform(Transform(pos, Quat()));
// 			pBox->SetMass(2.0f);
// 
// 			CMeshRenderNodePtr pRenderBox = new CMeshRenderNode;
// 			pRenderBox->AttachTo(pBox.get());
// 			pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(size)));
// 			pRenderBox->SetMesh(cube);
// 			pRenderBox->SetShaderProgram(0, "white.program");
// 		}
// 	}

	{
		CStaticMeshColliderNode* pPlaneCol = new CStaticMeshColliderNode;

		pPlaneCol->SetMesh(plane);

		CMeshRenderNode* pRenderBox = new CMeshRenderNode;
		pRenderBox->AttachTo(pPlaneCol);
		pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat()));
		pRenderBox->SetMesh(plane);
		pRenderBox->SetShaderProgram(0, "white.program");

		pPlaneCol->AttachTo(&rootNode);
		pPlaneCol->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(1000.0f, 1000.0f, 1000.0f)));
	}

	Transform t;
	t.position = Vec3(0.0f, -950.0f, 10.0f);
	//t.scale = Vec3(100, 100, 100);
	
		CBoxColliderNode* pBox1 = new CBoxColliderNode;
		{
		pBox1->SetSize(Vec3(100.0f));


		pBox1->AttachTo(&rootNode);
		pBox1->SetMass(2.0f);

		CMeshRenderNode* pRenderBox = new CMeshRenderNode;
		pRenderBox->AttachTo(pBox1);
		pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(100, 100, 100)));
		pRenderBox->SetMesh(cube);
		pRenderBox->SetShaderProgram(0, "white.program");

		pBox1->SetLocalTransform(t);

		gVars->pPhysicsSystem->SetDebugDraw( true );
	}

		CBoxColliderNode* pBox2 = new CBoxColliderNode;
		{
			pBox2->SetSize(Vec3(100.0f));


			pBox2->AttachTo(&rootNode);
			pBox2->SetMass(2.0f);

			CMeshRenderNode* pRenderBox = new CMeshRenderNode;
			pRenderBox->AttachTo(pBox2);
			pRenderBox->SetLocalTransform(Transform(Vec3::Zero, Quat(), Vec3(100,100,100)));
			pRenderBox->SetMesh(cube);
			pRenderBox->SetShaderProgram(0, "white.program");

			pBox2->SetLocalTransform(t);

		}

 	SpawnCube(Transform());


// 
//  	
	gVars->pResourceManager->UpdateResourceLoading();


		rootNode.Refresh();

		gVars->pPhysicsSystem->AddHingeConstraint(pBox1->m_rigidBodyHandle, pBox2->m_rigidBodyHandle, Vec3(5,0,0), Vec3(-5,0,0), Vec3(0, 1, 0), Vec3(0, 1, 0), 0, 1);


		static SGenotype gen;
		CreateGenotype(gen);
		
		BuildPhenotype(gen, ph, Vec3(500.0f, -950.0f, 40.0f));


// 		Transform houseT;
// 		pHouse->GetGlobalTransform(houseT);
// 		 		TColliderHandle houseShape = gVars->pPhysicsSystem->CreateStaticMeshCollider(CMeshPtr("Interior/house.msh")->GetTriMesh().get(), houseT);// ttt);
// 		 		TRigidBodyHandle houseRB = gVars->pPhysicsSystem->CreateRigidyBody(houseShape, Transform(),0.0f);
// 		 
// 		 			gVars->pPhysicsSystem->AddRigidyBody(houseRB);


		timer.Start();
}

void CEngine::Resize( uint width, uint height )
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

void CEngine::FrameUpdate()
{
	ph.ApplyLinkForce(1, 0.75f);
	float x = ph.GetJointAngle(0);

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

	//	pThird->ApplySkinningPose(pose);
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

	m_pRenderPipeline->mainCamera.transform.rotation = Quat::CreateRotZ(Degree(-45.0f));


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
// 
// 	bool bMovingCamera = gVars->pOperatingSystem->GetMouseButton(0);
// 	if (m_bMovingCamera != bMovingCamera)
// 	{
// 		//m_prevMousePos = gVars->pOperatingSystem->GetMousePos();
// 	}
// 	if ( m_bMovingCamera )
// 	{
// 		Vec2 mousePos = gVars->pOperatingSystem->GetMousePos();
// 		m_pRenderPipeline->mainCamera.transform.rotation.
// 	}

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
