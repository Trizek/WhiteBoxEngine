#include "SceneSerializer.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"
#include "Scene.h"

WHITEBOX_BEGIN

IResource*	CSceneSerializer::Load( ISerializer& serializer, const CResourceDescriptor& descriptor )
{
	CScene* pScene = new CScene;

	for(;;)
	{
		CSceneNode* pNode = nullptr;
		CSerializableFactory::Get().Serialize( pNode, serializer );
		if ( pNode )
		{
			pScene->nodes.push_back( pNode );
		}
		else
		{
			break;
		}
	}

	return pScene;
}

WHITEBOX_END
