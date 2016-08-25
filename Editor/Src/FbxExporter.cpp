#include "FbxExporter.h"

#include "Render/MeshHelper.h"
/*#include "System.h"*/
#include "FileSystem.h"

#pragma optimize( "", on ) 
#include "FbxConv.h"
#pragma optimize( "", off ) 

#include "Application.h"
#include "Animation/Skeleton.h"
#include "Animation/Animation.h"

SBone skel;

using namespace fbxconv;
using namespace WhiteBox;

WHITEBOX_BEGIN

extern Map< String, Transform > anim;


void ParseModelNode( Node* node, CSkeleton& skeleton, const String& parentName )
{
	Transform transf;

	transf.rotation = Quat(node->transform.rotation[3], node->transform.rotation[0], node->transform.rotation[1], node->transform.rotation[2]);
	transf.position = Vec3(node->transform.translation[0], node->transform.translation[1], node->transform.translation[2]);
	transf.scale = Vec3(node->transform.scale[0], node->transform.scale[1], node->transform.scale[2]);

	if (transf.rotation.w < 0.0f)
	{
		transf.rotation = -1.0f * transf.rotation;
	}

	String boneName = node->id.c_str();

	skeleton.AddBone( boneName, parentName, transf );

	for ( Node* child : node->children )
	{
		ParseModelNode( child, skeleton, boneName );
	}
}

void CFbxExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	log::Log log(new log::DefaultMessages(), -1);
	FbxConv conv(&log);

	Settings settings;
	settings.flipV = false;
	settings.packColors = false;
	settings.verbose = true;
	settings.maxNodePartBonesCount = 120;
	settings.maxVertexBonesCount = 4;
	settings.maxVertexCount = INT_MAX; //   (1 << 15) - 1;
	settings.maxIndexCount = INT_MAX; // (1 << 15) - 1;
	settings.outType = FILETYPE_AUTO;
	settings.inType = FILETYPE_FBX;
	settings.inFile = std::string( (assetFolder + filePath).c_str() );

	modeldata::Model *model = new modeldata::Model();
	if (!conv.load(&settings, model))
	{
		return;
	}

	conv.info(model);

	CSkeleton* pLastSkeleton = nullptr;

	for (Node* node : model->nodes)
	{
		CSkeleton* pSkeleton = new CSkeleton();
		ParseModelNode( node, *pSkeleton, "" );
		pSkeleton->ComputeGlobalPose(pSkeleton->m_localBindPose, pSkeleton->m_globalBindPose);


		String skeletonPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".skel";
		gVars->pFileSystem->CreateFileDir( skeletonPath );
		pSkeleton->SaveToFile( skeletonPath );

		if ( pLastSkeleton != nullptr )
		{
			delete pLastSkeleton;
		}

		pLastSkeleton = pSkeleton;
	}


	for (Animation* ani : model->animations)
	{
		CAnimation* pAnimation = new CAnimation();

		for (NodeAnimation* nodeAnim : ani->nodeAnimations)
		{
			int idx = pLastSkeleton->m_boneNameToIndex[nodeAnim->node->id.c_str()];
			if (idx < 0)
				continue;

			const std::vector< Keyframe* >& keyframes = nodeAnim->keyframes;

			Transform* trackKeys = new Transform[ keyframes.size() ];

			for (size_t iKey = 0; iKey < keyframes.size(); ++iKey)
			{
				const Keyframe& k = *(keyframes[ iKey ]);

				Transform transf;
				transf.rotation = Quat(k.rotation[3], k.rotation[0], k.rotation[1], k.rotation[2]);

				if (transf.rotation.w < 0.0f)
				{
					transf.rotation = -1.0f * transf.rotation;
				}

				transf.position = Vec3(k.translation[0], k.translation[1], k.translation[2]);
				transf.scale = Vec3(k.scale[0], k.scale[1], k.scale[2]);

				transf = pLastSkeleton->m_localBindPose.m_boneTransforms[idx].GetInverse() * transf;
				
				if (String(nodeAnim->node->id.c_str()).Contains("twist"))
				{
					transf = Transform();
				}

				trackKeys[ iKey ] = transf;
			}

			pAnimation->AddAnimationTack( new CAnimationTrack( CAnimationTrack::eKFF_Transform, trackKeys, keyframes.size() ), idx );
		}

		String animPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".anim";
		gVars->pFileSystem->CreateFileDir(animPath);
		pAnimation->SaveToFile(animPath);
		delete pAnimation;
	}

	if (pLastSkeleton != nullptr)
	{
		delete pLastSkeleton;
	}


	for (Mesh* mesh : model->meshes)
	{
		CMeshHelper mh;
		size_t vertexCount = mesh->vertexCount();
		size_t vertexFloatCount = mesh->vertexSize;
		mh.m_positionArray.reserve(vertexCount);
		mh.m_normalArray.reserve(vertexCount);

		size_t attCount = mesh->attributes.length();
		for (size_t att = 0; att < attCount; ++att)
		{
			size_t idx = mesh->attributes.get(att);


			unsigned int offset = 0;
			for (unsigned int i = 0; i < idx; i++)
				if (mesh->attributes.has(i))
					offset += (unsigned int)ATTRIBUTE_SIZE(i);
		

			switch (idx)
			{
			case ATTRIBUTE_POSITION:
			{
				for (size_t i = 0; i < vertexCount; ++i)
				{
					mh.AddPosition(*(Vec3*)(&mesh->vertices[vertexFloatCount * i]));



				}
				break;
			}
			case ATTRIBUTE_NORMAL:
			{
		
					for (size_t i = 0; i < vertexCount; ++i)
					{
						mh.AddNormal(*(Vec3*)(&mesh->vertices[offset + vertexFloatCount * i]));
					}
				break;
			}
			case ATTRIBUTE_BLENDWEIGHT0:
			case ATTRIBUTE_BLENDWEIGHT1:
			case ATTRIBUTE_BLENDWEIGHT2:
			case ATTRIBUTE_BLENDWEIGHT3:
			case ATTRIBUTE_BLENDWEIGHT4:
			case ATTRIBUTE_BLENDWEIGHT5:
			case ATTRIBUTE_BLENDWEIGHT6:
			case ATTRIBUTE_BLENDWEIGHT7:
			{
				size_t attributeIndex = idx - (size_t)ATTRIBUTE_BLENDWEIGHT0;
				for (size_t i = 0; i < vertexCount; ++i)
				{
					mh.AddBlendWeight( attributeIndex, *((SVertexBlendWeight*)(&mesh->vertices[offset + vertexFloatCount * i])) );
				}
				break;
			}
			}

		}

		for (MeshPart* meshPart : mesh->parts)
		{
			mh.AddMeshPart();
			CMeshPartHelper* part = mh.GetMeshPart(mh.m_meshParts.size()-1);

			for (unsigned short indice : meshPart->indices)
			{
				part->AddIndex(indice);
			}
		}

		String meshPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".msh";
		
		gVars->pFileSystem->CreateFileDir( meshPath );
		mh.SaveToFile( meshPath );
	}
}



WHITEBOX_END



#define FBX_EXPORTER 0
#if FBX_EXPORTER

#include <stdio.h>

#include <fbxsdk.h>
#include <fbxfilesdk/kfbxio/kfbxiosettings.h>
#include <fbxfilesdk/kfbxio/kfbximporter.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/kfbxplugins/kfbxscene.h>
#include <fbxfilesdk/fbxfilesdk_nsuse.h>

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif


void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene)
{
    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
    pSdkManager = KFbxSdkManager::Create();

    if (!pSdkManager)
    {
        WbLog( "Default", "Unable to create the FBX SDK manager\n");
        return;
    }

	// create an IOSettings object
	KFbxIOSettings * ios = KFbxIOSettings::Create(pSdkManager, IOSROOT );
	pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	KString lExtension = "so";
#endif
	pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

    // Create the entity that will hold the scene.
    pScene = KFbxScene::Create(pSdkManager,"");
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        WbLog( "Default", "Call to KFbxImporter::Initialize() failed.\n");
        WbLog( "Default", "Error returned: %s\n\n", lImporter->GetLastErrorString());

        if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
            lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
        {
            WbLog( "Default", "FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            WbLog( "Default", "FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    WbLog( "Default", "FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        WbLog( "Default", "FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        WbLog( "Default", "Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        WbLog( "Default", "    Number of Animation Stacks: %d\n", lAnimStackCount);
        WbLog( "Default", "    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        WbLog( "Default", "\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            WbLog( "Default", "    Animation Stack %d\n", i);
            WbLog( "Default", "         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            WbLog( "Default", "         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            WbLog( "Default", "         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            WbLog( "Default", "         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            WbLog( "Default", "\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
    {
        WbLog( "Default", "Please enter password: ");

        lPassword[0] = '\0';

        scanf("%s", lPassword);
        KString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
        {
            WbLog( "Default", "\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

void ParseMesh( KFbxMesh* pMesh )
{
    int controlPointsCount = pMesh->GetControlPointsCount();
    KFbxVector4* pControlPoints = pMesh->GetControlPoints();

    WbLog( "Default",  "Control points\n" );

    for ( int i = 0; i < controlPointsCount; i++ )
    {
		WbLog( "Default",  "(%.3f,%.3f,%.3f)\n", pControlPoints[i][0], pControlPoints[i][1], pControlPoints[i][2] );

/*
        for (int j = 0; j < pMesh->GetLayerCount(); j++)
        {
            KFbxLayerElementNormal* leNormals = pMesh->GetLayer(j)->GetNormals();
            if (leNormals)
            {
                if (leNormals->GetMappingMode() == KFbxLayerElement::eBY_CONTROL_POINT)
                {
                    char header[100];
                    sprintf(header, "            Normal Vector (on layer %d): ", j); 
                    if (leNormals->GetReferenceMode() == KFbxLayerElement::eDIRECT)
                        Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
                }
            }
        }*/
    }
}

void ParseNode( KFbxNode* pNode, const WhiteBox::String& resourceFolder, const WhiteBox::String& filePath )
{
	if ( pNode == NULL )
	{
		return;
	}
	
	KFbxNodeAttribute::EAttributeType attributeType;
    if(pNode->GetNodeAttribute() == NULL)
    {
        WbLog( "Default",  "NULL Node Attribute\n" );
    }
    else
    {
        attributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (attributeType)
        {
      /*  case KFbxNodeAttribute::eSKELETON:  
            DisplaySkeleton(pNode);
            break;*/

        case KFbxNodeAttribute::eMESH:      
			WbLog( "Default",  "Parsing mesh: %s\n", (char *)pNode->GetName() );
            ParseMesh( (KFbxMesh*)pNode->GetNodeAttribute() );
            break;
        }   
    }	
	
	
	for( int iChild = 0; iChild < pNode->GetChildCount(); iChild++)
	{
		ParseNode( pNode->GetChild( iChild ), resourceFolder, filePath );
	}	
}


WHITEBOX_BEGIN

void CFbxExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String completeFilePath = assetFolder + filePath;
	
	WbLog( "Default",  "Exporting FBX %s\n", completeFilePath.c_str() );
	
    KFbxSdkManager* pSdkManager = NULL;
    KFbxScene* pScene = NULL;

    // Prepare the FBX SDK.
    InitializeSdkObjects( pSdkManager, pScene );
	
	if ( !LoadScene( pSdkManager, pScene, completeFilePath.c_str() ) || pScene == NULL )
	{
		WbLog( "Default",  "FBX loading scene failed !\n" );
	}	
	
	ParseNode( pScene->GetRootNode(), resourceFolder, filePath );

	
/*	TFileHandle file = gSystem->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
	
	CMeshHelper meshHelper;
	size_t meshPartCount = 0;
	size_t vertexCount = 0;

	char buffer[256];
	while( ReadWord( file, buffer ) )
	{
		if ( strcmp( buffer, "#" ) == 0 || strcmp( buffer, "mtllib" ) == 0 )
		{
			SkipLine( file );
		}
		else if ( strcmp( buffer, "g" ) == 0 )
		{
			meshHelper.AddMeshPart();
			++meshPartCount;
		}
		else if ( strcmp( buffer, "v" ) == 0 )
		{
			Vec3 pos;
			if ( ReadWord( file, buffer ) ){ pos.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ pos.y = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ pos.z = ToFloat( buffer ); }
			
			meshHelper.AddPosition( pos );
			
			++vertexCount;
		}
		else if ( strcmp( buffer, "vn" ) == 0 )
		{
			Vec3 normal;
			if ( ReadWord( file, buffer ) ){ normal.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ normal.y = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ normal.z = ToFloat( buffer ); }
			
			meshHelper.AddNormal( normal );
		}
		else if ( strcmp( buffer, "vt" ) == 0 )
		{
			Vec2 uv;
			if ( ReadWord( file, buffer ) ){ uv.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ uv.y = ToFloat( buffer ); }
			
			meshHelper.AddUV0( uv );
		}			
		else if ( meshPartCount > 0 && strcmp( buffer, "f" ) == 0 )
		{
			for( int index = 0 ; index < 3 ; ++index )
			{
				if ( ReadWord( file, buffer ) )
				{
					long int face = ToInt( buffer );
					uint index;
					if ( face < 0 )
					{
						index = (uint)(vertexCount + face);
					}
					else
					{
						index = (uint)(face - 1);
					}				
					meshHelper.GetMeshPart( meshPartCount - 1 )->AddIndex( index );
				}
			}
		}
		else if ( meshPartCount > 0 && strcmp( buffer, "usemtl" ) == 0 )
		{
			if ( ReadWord( file, buffer ) )
			{
				String materialName = resourceFolder + filePath.get_path_base() + buffer + ".mat";
				meshHelper.GetMeshPart( meshPartCount - 1 )->SetMaterialName( materialName );
			}
		}		
	}
	
	String meshPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".msh";
	gSystem->pFileSystem->CreateFileDir( meshPath );
	meshHelper.SaveToFile( meshPath );	
	
	gSystem->pFileSystem->CloseFile( file );*/

}

#endif