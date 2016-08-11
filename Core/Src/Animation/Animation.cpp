#include "Animation/Animation.h"

WHITEBOX_BEGIN

CAnimation::CAnimation()
	: m_length(0.0f){}

void	CAnimation::AddAnimationTack( CAnimationTrack* pAnimTrack )
{
	m_animationTracks.push_back( pAnimTrack );
	if ( pAnimTrack )
	{
		m_length = Max( m_length, pAnimTrack->GetLength() );
	}
}

void	CAnimation::AccumulateSamplePose( float animTime, float animWeight, CPose& accPose, float& accWeight ) const
{
	size_t bonesCount = Min( accPose.m_boneTransforms.size(), m_animationTracks.size() );
	for( size_t iBone = 0 ; iBone < bonesCount ; ++iBone )
	{
		CAnimationTrack* pAnimTrack = m_animationTracks[ iBone ];
		if ( pAnimTrack == NULL )
		{
			continue;
		}
		
		pAnimTrack->AccumulateSample( animTime, animWeight, accPose.m_boneTransforms[ iBone ] );
	}
	accWeight += animWeight;
}

WHITEBOX_END
