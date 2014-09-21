#ifndef __ANIMATIONTRACK_H__
#define __ANIMATIONTRACK_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CAnimationTrack
{
public:
	static const int AnimationFrameRate = 30;

	enum EKeyFrameFormat
	{
		eKFF_Transform = 0,
		eKFF_Position,
		eKFF_Quaternion48Bits,
	};
	
	CAnimationTrack( EKeyFrameFormat keyFrameFormat, void* keyFrames, size_t keyFrameCount );
	~CAnimationTrack();
	
	void	GetKeyFrame( size_t index, Transform& keyFrame ) const;
	void	Sample( float animTime, Transform& fromKeyFrame, Transform& toKeyFrame, float& toWeight ) const;
	void	AccumulateSample( float animTime, float sampleWeight, Transform& accTransform, float& accWeight ) const;

private:
	EKeyFrameFormat	m_keyFrameFormat;
	void*			m_keyFrames;
	size_t			m_keyFrameCount;
};

WHITEBOX_END

#endif
