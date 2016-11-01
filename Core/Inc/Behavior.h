#ifndef __BEHAVIOR_H__
#define __BEHAVIOR_H__

#include "SceneNode.h"

WHITEBOX_BEGIN

class IBehavior : public CSceneNode
{
public:
	IBehavior();
	virtual ~IBehavior();

	virtual void	Start() {}
	virtual void	Update(float frameTime) {}

	virtual void	Refresh() override
	{
		Start();
		CSceneNode::Refresh();
	}
};

WHITEBOX_END

#endif
