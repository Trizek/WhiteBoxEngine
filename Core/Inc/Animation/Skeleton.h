#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CBone
{
public:
	CBone();
	CBone( const String& name, int index, int parentIndex );
		
	const String&	GetName() const;
	int				GetIndex() const;
	int				GetParentIndex() const;

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
	
	
	void AddBone( const String& boneName, const String& parentName, const Transform& localTransform );
	void ComputeInvertedGlobalBindPose();
	void ConvertFromBindToLocalSpace( CPose& pose );
	void ComputeGlobalPose( const CPose& localPose, CPose& globalPose ) const;
	void ComputeSkinningPose( const CPose& globalPose, CPose& skinPose ) const;

private:
	TBoneArray	m_bones;
	CPose		m_invertedGlobalBindPose;
	CPose		m_localBindPose;
	
	typedef Map< String, size_t > TBoneNameToIndexMap;
	TBoneNameToIndexMap	m_boneNameToIndex;
};


WHITEBOX_END

#endif
