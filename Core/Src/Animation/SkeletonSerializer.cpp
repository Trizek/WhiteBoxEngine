#include "Animation/SkeletonSerializer.h"

#include "DataStream.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

IResource*	CSkeletonSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	CSkeleton* pSkeleton = new CSkeleton();

	int boneCount = 0;
	dataStream.Read( boneCount );


	CSkeleton::TBoneNameToIndexMap& boneNameToIndexMap = pSkeleton->GetBoneNameToIndexMap();
	TBoneInfoArray& boneInfos = pSkeleton->GetBones();
	boneInfos.resize( (size_t)boneCount );
	for (int iBone = 0; iBone < boneCount; ++iBone)
	{
		String boneName;
		int boneIndex, parentIndex;
		
		dataStream.ReadString( boneName );
		dataStream.Read( boneIndex );
		dataStream.Read( parentIndex );
	
		boneInfos[ iBone ] = CBoneInfo( boneName, boneIndex, parentIndex );
		boneNameToIndexMap[ boneName ] = boneIndex;
	}

	CPose& bindPose = pSkeleton->GetGlobalBindPose();
	bindPose.m_boneTransforms.resize( (size_t)boneCount );
	for ( int iBone = 0; iBone < boneCount; ++iBone )
	{
		dataStream.Read( bindPose.m_boneTransforms[ iBone ] );
	}

	pSkeleton->ComputeInvertedGlobalBindPose();
	pSkeleton->ComputeLocalBindPose();

	return pSkeleton;
}

WHITEBOX_END
