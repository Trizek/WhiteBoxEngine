#include "Animation/AnimationTrack.h"

WHITEBOX_BEGIN




CAnimationTrack::CAnimationTrack( EKeyFrameFormat keyFrameFormat, void* keyFrames, size_t keyFrameCount )
	: m_keyFrameFormat(keyFrameFormat)
	, m_keyFrames(keyFrames)
	, m_keyFrameCount(keyFrameCount){}
 
CAnimationTrack::~CAnimationTrack()
{
	if ( m_keyFrames )
	{
		delete (char*)m_keyFrames;
	}
}

float	CAnimationTrack::GetLength() const
{
	return float(m_keyFrameCount - 1) / float(AnimationFrameRate);
}

void	CAnimationTrack::GetKeyFrame( size_t index, Transform& keyFrame ) const
{
	switch( m_keyFrameFormat )
	{
		case eKFF_Transform:
		{
			keyFrame = ((Transform const*)m_keyFrames)[ index ];
		}
		break;
		case eKFF_Position:
		{
			keyFrame.position = ((Vec3 const*)m_keyFrames)[ index ];
			keyFrame.rotation = Quat();
			keyFrame.scale = Vec3( 1.0f );
		}
		break;
		case eKFF_Quaternion48Bits:
		{
			short const* quatVals = ((short const*)m_keyFrames) + 3 * index;
			keyFrame.position = Vec3();
			Quat::DecodeQuat( quatVals[ 0 ], quatVals[ 1 ], quatVals[ 2 ], keyFrame.rotation );
			keyFrame.scale = Vec3( 1.0f );			
		}
		break;
	}
}

void	CAnimationTrack::Sample( float animTime, Transform& fromKeyFrame, Transform& toKeyFrame, float& weight ) const
{
	size_t fromKeyIndex = size_t(animTime * AnimationFrameRate) % (m_keyFrameCount - 1);
	size_t toKeyIndex = ( fromKeyIndex + 1 ) % (m_keyFrameCount - 1);// Min( fromKeyIndex + 1, m_keyFrameCount - 1 );

	GetKeyFrame( fromKeyIndex, fromKeyFrame );
	GetKeyFrame( toKeyIndex, toKeyFrame );
	weight = animTime * AnimationFrameRate - ((float)fromKeyIndex);
}

void	CAnimationTrack::AccumulateSample( float animTime, float sampleWeight, Transform& accTransform ) const
{
	float weight;
	Transform fromKeyFrame, toKeyFrame;
	Sample( animTime, fromKeyFrame, toKeyFrame, weight );

	if ( (accTransform.rotation | fromKeyFrame.rotation) < 0.0f)
	{
		fromKeyFrame.rotation = -1.0f * fromKeyFrame.rotation;
	}

	if ( (fromKeyFrame.rotation | toKeyFrame.rotation) < 0.0f)
	{
		toKeyFrame.rotation = -1.0f * toKeyFrame.rotation;
	}

	//accTransform = accTransform + sampleWeight * fromKeyFrame;

	//return;
	accTransform = accTransform + sampleWeight * (1.0f - weight) * fromKeyFrame + sampleWeight * weight * toKeyFrame;
}



WHITEBOX_END
