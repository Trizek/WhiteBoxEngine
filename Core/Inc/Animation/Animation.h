#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "BaseTypes.h"

#include "AnimationTrack.h"
#include "Skeleton.h"
#include "ResourcePointer.h"

WHITEBOX_BEGIN

class CAnimation : public IResource
{
public:
	DECLARE_RESOURCE_TYPE(CAnimation);

	CAnimation();

	void				AddAnimationTack( CAnimationTrack* pAnimTrack, int index = -1 );
	
	void				AccumulateSamplePose( float animTime, float animWeight, CPose& accPose, float& accWeight ) const;

	void				SaveToFile( const String& filePath ) const;
/*
private:*/
	typedef std::vector< CSmartPointer<CAnimationTrack> >	TAnimationTrackArray;
	TAnimationTrackArray					m_animationTracks;
	
	float									m_length;
};

DECLARE_RESOURCE_POINTER(CAnimation);

WHITEBOX_END



#endif
