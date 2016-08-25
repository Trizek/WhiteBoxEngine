#include "Animation/AnimationSerializer.h"

#include "DataStream.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

IResource*	CAnimationSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	CAnimation* pAnimation = new CAnimation();

	float length = 0.0f;
	dataStream.Read( length );

	int trackCount = 0;
	dataStream.Read( trackCount );

	for( int iTrack = 0; iTrack < trackCount; ++iTrack )
	{
		int keyFrameCount = 0;
		dataStream.Read( keyFrameCount );

		if ( keyFrameCount == 0 )
		{
			continue;
		}
	
		int keyFormat = 0;
		dataStream.Read( keyFormat );

		Transform* keyFrames = new Transform[ keyFrameCount ];
		dataStream.Read( sizeof(Transform), keyFrameCount, keyFrames );

		pAnimation->AddAnimationTack( new CAnimationTrack( (CAnimationTrack::EKeyFrameFormat)keyFormat, keyFrames, keyFrameCount ), iTrack );
	}

	return pAnimation;
}

WHITEBOX_END
