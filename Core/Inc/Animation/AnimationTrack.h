#ifndef __ANIMATIONTRACK_H__
#define __ANIMATIONTRACK_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CAnimationTrack : public CSmartPointerData
{
public:
	static const int AnimationFrameRate = 30;

	enum EKeyFrameFormat
	{
		eKFF_Transform = 0,
		eKFF_Position,
		eKFF_Quaternion48Bits,
	};
	
	CAnimationTrack()
		: m_keyFrames(nullptr){}
	CAnimationTrack( EKeyFrameFormat keyFrameFormat, void* keyFrames, size_t keyFrameCount );
	~CAnimationTrack();
	
	float	GetLength() const;
	
	void	GetKeyFrame( size_t index, Transform& keyFrame ) const;
	void	Sample( float animTime, Transform& fromKeyFrame, Transform& toKeyFrame, float& toWeight ) const;
	void	AccumulateSample( float animTime, float sampleWeight, Transform& accTransform ) const;
	
public:
	EKeyFrameFormat	m_keyFrameFormat;
	void*			m_keyFrames;
	size_t			m_keyFrameCount;
};

WHITEBOX_END

#endif
