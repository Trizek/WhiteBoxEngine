#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CBone
{
public:
	CBone()
		: m_index(-1), m_parentIndex(-1){}

	CBone( const String& name, int index, int parentIndex )
		: m_name(name)
		, m_index(index)
		, m_parentIndex(parentIndex){}
		
	const String&	GetName() const
	{
		return m_name;
	}
	
	int	GetIndex() const
	{
		return m_index;
	}
	
	int	GetParentIndex() const
	{
		return m_parentIndex;
	}

protected:
	String	m_name;
	int		m_index;
	int		m_parentIndex;
};

typedef std::vector< CBone > TBoneArray;

class CPose
{
public:
	std::vector< Transform >	m_boneTransforms;
};

class CSkeleton
{
public:
	TBoneArray&	GetBones(){ return m_bones; }
	CPose&	GetInvertedGlobalBindPose(){ return m_invertedGlobalBindPose; }
	CPose&	GetLocalBindPose(){ return m_localBindPose; }
	
	
	void AddBone( const String& boneName, const String& parentName, const Transform& localTransform )
	{
		int parentIndex = -1;
		TBoneNameToIndexMap::iterator parentIt = m_boneNameToIndex.find( parentName );
		if ( parentIt != m_boneNameToIndex.end() )
		{
			parentIndex = parentIt->second;
		}
		
		int boneIndex = (int)m_bones.size();
		
		m_bones.push_back( CBone( boneName, boneIndex, parentIndex ) );
		m_localBindPose.m_boneTransforms.push_back( localTransform );
		
		m_boneNameToIndex[ boneName ] = boneIndex;
	}
	
	void ComputeInvertedGlobalBindPose()
	{
		m_invertedGlobalBindPose.m_boneTransforms.resize( m_localBindPose.m_boneTransforms.size() );
		
		// Compute global pose
		ComputeGlobalPose( m_localBindPose, m_invertedGlobalBindPose );
		
		// Invert it
		for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
		{
			m_invertedGlobalBindPose.m_boneTransforms[ iBone ] = !m_invertedGlobalBindPose.m_boneTransforms[ iBone ];
		}		
	}
	
	void ConvertFromBindToLocalSpace( CPose& pose )
	{
		pose.m_boneTransforms.resize( m_bones.size() );
	
		for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
		{
			pose.m_boneTransforms[ iBone ] = m_localBindPose.m_boneTransforms[ iBone ] * pose.m_boneTransforms[ iBone ];
		}		
	}
	
	void ComputeGlobalPose( const CPose& localPose, CPose& globalPose ) const
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
	
	void ComputeSkinningPose( const CPose& globalPose, CPose& skinPose ) const
	{		
		skinPose.m_boneTransforms.resize( m_bones.size() );
	
		for( size_t iBone = 0 ; iBone < m_localBindPose.m_boneTransforms.size() ; ++iBone )
		{
			skinPose.m_boneTransforms[ iBone ] = globalPose.m_boneTransforms[ iBone ] * m_invertedGlobalBindPose.m_boneTransforms[ iBone ];
		}
	}

private:
	TBoneArray	m_bones;
	CPose		m_invertedGlobalBindPose;
	CPose		m_localBindPose;
	
	typedef Map< String, size_t > TBoneNameToIndexMap;
	TBoneNameToIndexMap	m_boneNameToIndex;
};


// Chose anim to play
// Deduce anim skel pose (in local bind pose space ) (several for each layers)
// Deduce local anim skel pose (in local anim space) and global anim skel pose
// Post-process (if change to local anim skel pose, responsible of updating global anim pose)
// Deduce skin pose from final global pose, send to software skinning or skinning shader

class CSkeletonInstance
{
	CSkeleton* m_pSkeleton;

	CPose m_cacheSkinningPose;
};

WHITEBOX_END

#endif