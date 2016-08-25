#include "Animation/Animation.h"
#include "FileSystem.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

CAnimation::CAnimation()
	: m_length(0.0f){}

void	CAnimation::AddAnimationTack( CAnimationTrack* pAnimTrack, int index )
{
	size_t newSize = (index > 0) ? index + 1 : m_animationTracks.size() + 1;
	if ( newSize > m_animationTracks.size() )
	{
		m_animationTracks.resize( newSize );
	}

	m_animationTracks[ index ] = pAnimTrack;
	m_length = Max( m_length, pAnimTrack->GetLength() );
}

void	CAnimation::AccumulateSamplePose( float animTime, float animWeight, CPose& accPose, float& accWeight ) const
{
	while (animTime > m_length)
		animTime -= m_length;

	size_t bonesCount = Min( accPose.m_boneTransforms.size(), m_animationTracks.size() );
	for( size_t iBone = 0 ; iBone < bonesCount ; ++iBone )
	{
		CSmartPointer<CAnimationTrack> pAnimTrack = m_animationTracks[ iBone ];
		if ( pAnimTrack )
		{
			Transform accTransf = accPose.m_boneTransforms[ iBone ];
			pAnimTrack->AccumulateSample( animTime, animWeight, accTransf );

// 			if ( ((accWeight + animWeight) >= 1.0f) && (accTransf.rotation.SqrLength() < 0.1f * 0.1f) )
// 			{
// 				accTransf.rotation = (1.0f + animWeight) * accPose.m_boneTransforms[ iBone ].rotation;
// 			}

			accPose.m_boneTransforms[ iBone ] = accTransf;
		}
		else
		{
			accPose.m_boneTransforms[ iBone ] = accPose.m_boneTransforms[iBone] + animWeight * Transform();
		}
	}
	accWeight += animWeight;
}


void CAnimation::SaveToFile( const String& filePath ) const
{
	TFileHandle file = gVars->pFileSystem->OpenFile( filePath.c_str(), false, true );

	gVars->pFileSystem->Write( file, sizeof(float), 1, &m_length );

	int trackCount = (int)m_animationTracks.size();
	gVars->pFileSystem->Write( file, sizeof(int), 1, &trackCount );

	for( CSmartPointer<CAnimationTrack> pAnimTrack : m_animationTracks )
	{
		if ( pAnimTrack.get() == nullptr )
		{
			int keyCount = 0;
			gVars->pFileSystem->Write( file, sizeof(int), 1, &keyCount );
	
			continue;
		}

		int keyCount = (int)pAnimTrack->m_keyFrameCount;
		gVars->pFileSystem->Write( file, sizeof(int), 1, &keyCount );

		int keyFormat = (int)pAnimTrack->m_keyFrameFormat;
		gVars->pFileSystem->Write( file, sizeof(int), 1, &keyFormat );

		gVars->pFileSystem->Write( file, sizeof(Transform), keyCount, pAnimTrack->m_keyFrames );
	}

	gVars->pFileSystem->CloseFile( file );
}

WHITEBOX_END
