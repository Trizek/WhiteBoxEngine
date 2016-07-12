#include "FbxExporter.h"

#include "Render/MeshHelper.h"
#include "System.h"
#include "FileSystem.h"

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
        printf("Unable to create the FBX SDK manager\n");
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
        printf("Call to KFbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

        if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
            lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
        {
            printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        printf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            printf("    Animation Stack %d\n", i);
            printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            printf("\n");
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
        printf("Please enter password: ");

        lPassword[0] = '\0';

        scanf("%s", lPassword);
        KString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
        {
            printf("\nPassword is wrong, import aborted.\n");
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

    printf( "Control points\n" );

    for ( int i = 0; i < controlPointsCount; i++ )
    {
		printf( "(%.3f,%.3f,%.3f)\n", pControlPoints[i][0], pControlPoints[i][1], pControlPoints[i][2] );

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
        printf( "NULL Node Attribute\n" );
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
			printf( "Parsing mesh: %s\n", (char *)pNode->GetName() );
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
	
	printf( "Exporting FBX %s\n", completeFilePath.c_str() );
	
    KFbxSdkManager* pSdkManager = NULL;
    KFbxScene* pScene = NULL;

    // Prepare the FBX SDK.
    InitializeSdkObjects( pSdkManager, pScene );
	
	if ( !LoadScene( pSdkManager, pScene, completeFilePath.c_str() ) || pScene == NULL )
	{
		printf( "FBX loading scene failed !\n" );
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

#else

WHITEBOX_BEGIN

void CFbxExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{}
#endif

WHITEBOX_END
