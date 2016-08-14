#include "Animation/Skeleton.h"

WHITEBOX_BEGIN

CBone::CBone()
	: m_index(-1), m_parentIndex(-1){}

CBone::CBone( const String& name, int index, int parentIndex )
	: m_name(name)
	, m_index(index)
	, m_parentIndex(parentIndex){}
	
const String&	CBone::GetName() const
{
	return m_name;
}

int	CBone::GetIndex() const
{
	return m_index;
}

int	CBone::GetParentIndex() const
{
	return m_parentIndex;
}

void CSkeleton::AddBone( const String& boneName, const String& parentName, const Transform& localTransform )
{
	int parentIndex = -1;
	TBoneNameToIndexMap::FindRes parentIt = m_boneNameToIndex.FindElement( parentName );
	if ( parentIt != nullptr )
	{
		parentIndex = (int)*parentIt;
	}
	
	int boneIndex = (int)m_bones.size();
	
	m_bones.push_back( CBone( boneName, boneIndex, parentIndex ) );
	m_localBindPose.m_boneTransforms.push_back( localTransform );
	
	m_boneNameToIndex[ boneName ] = boneIndex;
}

void CSkeleton::ComputeInvertedGlobalBindPose()
{
	m_invertedGlobalBindPose.m_boneTransforms.resize( m_bones.size() );
	
	// Compute global pose
	ComputeGlobalPose( m_localBindPose, m_invertedGlobalBindPose );
	
	// Invert it
	for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
	{
		m_invertedGlobalBindPose.m_boneTransforms[ iBone ] = !m_invertedGlobalBindPose.m_boneTransforms[ iBone ];
	}		
}

void CSkeleton::ConvertFromBindToLocalSpace( CPose& pose )
{
	pose.m_boneTransforms.resize( m_bones.size() );

	for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
	{
		pose.m_boneTransforms[ iBone ] = m_localBindPose.m_boneTransforms[ iBone ] * pose.m_boneTransforms[ iBone ];
	}		
}

void CSkeleton::ComputeGlobalPose( const CPose& localPose, CPose& globalPose ) const
{
	globalPose.m_boneTransforms.resize( m_bones.size() );
	
	for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
	{
		Transform globalTransf = localPose.m_boneTransforms[ iBone ];
		int parentIndex = m_bones[ iBone ].GetParentIndex();
		if ( parentIndex >= 0.0f )
		{
			globalTransf = globalPose.m_boneTransforms[ parentIndex ] * globalTransf;
		}
		globalPose.m_boneTransforms[ iBone ] = globalTransf;
	}
}	

void CSkeleton::ComputeSkinningPose( const CPose& globalPose, CPose& skinPose ) const
{		
	skinPose.m_boneTransforms.resize( m_bones.size() );

	for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
	{
		skinPose.m_boneTransforms[ iBone ] = globalPose.m_boneTransforms[ iBone ] * m_invertedGlobalBindPose.m_boneTransforms[ iBone ];
	}
}

WHITEBOX_END
