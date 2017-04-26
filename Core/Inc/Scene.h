#ifndef __SCENE_H__
#define __SCENE_H__

#include "BaseTypes.h"
#include "SceneNode.h"
#include "Resource.h"

WHITEBOX_BEGIN

class CScene : public IResource
{
public:
	DECLARE_RESOURCE_TYPE(CScene);

public:
	void Refresh()
	{
		for( CSceneNodePtr& node : nodes )
		{
			node->Refresh();
		}
	}

	std::vector<CSceneNodePtr>	nodes;
};

DECLARE_RESOURCE_POINTER(CScene);

WHITEBOX_END

#endif