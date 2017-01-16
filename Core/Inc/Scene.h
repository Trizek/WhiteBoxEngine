#ifndef __SCENE_H__
#define __SCENE_H__

#include "BaseTypes.h"
#include "SceneNode.h"

WHITEBOX_BEGIN

class CScene : public CSmartPointerData
{
public:
	void	AddNode( CSceneNodePtr node )
	{
		node->AttachTo( &rootNode, ENodeAttachMode::ConserveGlobalTransform );
	}

private:
	CSpatialNode	rootNode;
};

DEFINE_SMART_PTR(CScene);

WHITEBOX_END

#endif