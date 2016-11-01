#include "Behavior.h"
#include "BehaviorManager.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

IBehavior::IBehavior()
{
	gVars->pBehaviorManager->AddBehavior( this );
}

IBehavior::~IBehavior()
{
	gVars->pBehaviorManager->RemoveBehavior( this );
}

WHITEBOX_END
