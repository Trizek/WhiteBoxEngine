#include "Animation/Skeleton.h"
#include "FileSystem.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

CBoneInfo::CBoneInfo()
	: m_index(-1), m_parentIndex(-1){}

CBoneInfo::CBoneInfo( const String& name, int index, int parentIndex )
	: m_name(name)
	, m_index(index)
	, m_parentIndex(parentIndex){}
	
const String&	CBoneInfo::GetName() const
{
	return m_name;
}

int	CBoneInfo::GetIndex() const
{
	return m_index;
}

int	CBoneInfo::GetParentIndex() const
{
	return m_parentIndex;
}

void CSkeleton::AddBone( const String& boneName, const String& parentName, const Transform& globalTransform )
{
	Transform transform = globalTransform;

	int parentIndex = -1;
	TBoneNameToIndexMap::FindRes parentIt = m_boneNameToIndex.FindElement( parentName );
	if ( parentIt != nullptr )
	{
		parentIndex = (int)*parentIt;
	}
	else
	{
		transform = Transform();
	}
	
	int boneIndex = (int)m_boneInfos.size();
	
	m_boneInfos.push_back( CBoneInfo( boneName, boneIndex, parentIndex ) );
	m_localBindPose.m_boneTransforms.push_back( transform );
	
	m_boneNameToIndex[ boneName ] = boneIndex;
}

void CSkeleton::ComputeLocalBindPose()
{
	m_localBindPose.m_boneTransforms.resize( m_boneInfos.size() );
	
	for( size_t iBone = 0 ; iBone < m_globalBindPose.m_boneTransforms.size() ; ++iBone )
	{
		int parent = m_boneInfos[ iBone ].GetParentIndex();
		if ( parent >= 0 )
		{
			m_localBindPose.m_boneTransforms[ iBone ] = !m_globalBindPose.m_boneTransforms[ parent ] * m_globalBindPose.m_boneTransforms[ iBone ];
		}
		else
		{
			m_localBindPose.m_boneTransforms[ iBone ] = m_globalBindPose.m_boneTransforms[ iBone ];
		}
	}		
}


void CSkeleton::ComputeInvertedGlobalBindPose()
{
	m_invertedGlobalBindPose.m_boneTransforms.resize( m_boneInfos.size() );
	
	// Invert it
	for( size_t iBone = 0 ; iBone < m_globalBindPose.m_boneTransforms.size() ; ++iBone )
	{
		m_invertedGlobalBindPose.m_boneTransforms[ iBone ] = !m_globalBindPose.m_boneTransforms[ iBone ];
	}		
}

void CSkeleton::ConvertFromBindToLocalSpace( CPose& pose )
{
	pose.m_boneTransforms.resize( m_boneInfos.size() );

	for( size_t iBone = 0 ; iBone < m_globalBindPose.m_boneTransforms.size() ; ++iBone )
	{
		pose.m_boneTransforms[ iBone ] = m_localBindPose.m_boneTransforms[ iBone ] * pose.m_boneTransforms[ iBone ];
	}		
}

void CSkeleton::ComputeGlobalPose( const CPose& localPose, CPose& globalPose ) const
{
 	globalPose.m_boneTransforms.resize( m_boneInfos.size() );
 	
 	for( size_t iBone = 0 ; iBone < m_globalBindPose.m_boneTransforms.size() ; ++iBone )
 	{
 		Transform globalTransf = localPose.m_boneTransforms[ iBone ];
 		int parentIndex = m_boneInfos[ iBone ].GetParentIndex();
 		if ( parentIndex >= 0 )
 		{
 			globalTransf = globalPose.m_boneTransforms[ parentIndex ] * globalTransf;
 		}
 		globalPose.m_boneTransforms[ iBone ] = globalTransf;
 	}
}	

void CSkeleton::ComputeSkinningPose( const CPose& globalPose, CPose& skinPose ) const
{		
	skinPose.m_boneTransforms.resize( m_boneInfos.size() );

	for( size_t iBone = 0 ; iBone < m_globalBindPose.m_boneTransforms.size() ; ++iBone )
	{
		skinPose.m_boneTransforms[ iBone ] = globalPose.m_boneTransforms[ iBone ] * m_invertedGlobalBindPose.m_boneTransforms[ iBone ];
	}
}

void CSkeleton::SaveToFile( const String& filePath ) const
{
	TFileHandle file = gVars->pFileSystem->OpenFile( filePath.c_str(), false, true );

	int boneCount = (int)m_boneInfos.size();
	gVars->pFileSystem->Write( file, sizeof(int), 1, &boneCount );

	for (int iBone = 0; iBone < boneCount; ++iBone)
	{
		const CBoneInfo& boneInfo = m_boneInfos[ iBone ];
		gVars->pFileSystem->WriteString( file, boneInfo.GetName() );

		int boneIndex = boneInfo.GetIndex();
		gVars->pFileSystem->Write( file, 1, sizeof(int), &boneIndex );

		int boneParentIndex = boneInfo.GetParentIndex();
		gVars->pFileSystem->Write( file, 1, sizeof(int), &boneParentIndex );
	}

	for (int iBone = 0; iBone < boneCount; ++iBone)
	{
		gVars->pFileSystem->Write( file, sizeof(Transform), 1, &m_globalBindPose.m_boneTransforms[ iBone ] );
	}

	gVars->pFileSystem->CloseFile( file );
}

WHITEBOX_END
