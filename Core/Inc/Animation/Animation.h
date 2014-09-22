#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "BaseTypes.h"

#include "AnimationTrack.h"
#include "Skeleton.h"

WHITEBOX_BEGIN

class CAnimation
{
public:
	CAnimation();

	void	AddAnimationTack( CAnimationTrack* pAnimTrack );
	
	void	AccumulateSamplePose( float animTime, float animWeight, CPose& accPose, float& accWeight ) const;

private:
	typedef std::vector< CAnimationTrack* >	TAnimationTrackArray;
	TAnimationTrackArray					m_animationTracks;
	
	float									m_length;
};

WHITEBOX_END

#endif
