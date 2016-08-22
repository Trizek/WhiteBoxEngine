#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "BaseTypes.h"
#include "ResourcePointer.h"

WHITEBOX_BEGIN

class CBoneInfo
{
public:
	CBoneInfo();
	CBoneInfo( const String& name, int index, int parentIndex );
		
	const String&	GetName() const;
	int				GetIndex() const;
	int				GetParentIndex() const;

protected:
	String	m_name;
	int		m_index;
	int		m_parentIndex;
};

typedef std::vector< CBoneInfo > TBoneInfoArray;

class CPose
{
public:
	std::vector< Transform >	m_boneTransforms;
};

class CSkeleton : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CSkeleton );

	typedef Map< String, size_t > TBoneNameToIndexMap;

	TBoneInfoArray&	GetBones(){ return m_boneInfos; }
	CSkeleton::TBoneNameToIndexMap& GetBoneNameToIndexMap() { return m_boneNameToIndex; }
	CPose&	GetInvertedGlobalBindPose(){ return m_invertedGlobalBindPose; }
	CPose&	GetGlobalBindPose(){ return m_globalBindPose; }
	
	
	void AddBone( const String& boneName, const String& parentName, const Transform& globalTransform );
	void ComputeLocalBindPose();
	void ComputeInvertedGlobalBindPose();
	void ConvertFromBindToLocalSpace( CPose& pose );
	void ComputeGlobalPose( const CPose& localPose, CPose& globalPose ) const;
	void ComputeSkinningPose( const CPose& globalPose, CPose& skinPose ) const;

	void	SaveToFile( const String& filePath ) const;

private:
	TBoneInfoArray	m_boneInfos;
	CPose			m_invertedGlobalBindPose;
	CPose			m_globalBindPose;
	CPose			m_localBindPose;

	TBoneNameToIndexMap	m_boneNameToIndex;
};

DECLARE_RESOURCE_POINTER( CSkeleton );

WHITEBOX_END

#endif
