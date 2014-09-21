#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "BaseTypes.h"

#include "AnimationTrack.h"

WHITEBOX_BEGIN

class CAnimation
{
public:	
	void	AddAnimationTack( CAnimationTrack* pAnimTrack );
	
	void	AccumulatePose( float animTime, float animWeight, CPose& accPose, float& accWeight ) const;

private:
	typedef std::vector< CAnimationTrack* >	TAnimationTrackArray;
	TAnimationTrackArray					m_animationTracks;
	
	float									m_length;
};

WHITEBOX_END

#endif
