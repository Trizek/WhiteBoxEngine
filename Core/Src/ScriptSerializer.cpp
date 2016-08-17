#include "ScriptSerializer.h"

WHITEBOX_BEGIN

CScriptNodeReader::CScriptNodeReader( SScriptNodePtr pRootNode )
{
	if ( pRootNode )
	{
		SGroup group;
		group.m_pScriptNode = pRootNode;
		group.m_curGroupIndex = 0;
		m_groupStack.push_back( group );
	}
}

bool CScriptNodeReader::IsReading() const
{
	return true;
}

bool CScriptNodeReader::BeginGroup( const String& name )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SGroup& parentGroup = m_groupStack.back();

	if ( parentGroup.m_curGroupName != name )
	{
		parentGroup.m_curGroupName = name;
		parentGroup.m_curGroupIndex = 0;
	}
	else
	{
		++parentGroup.m_curGroupIndex;
	}

	SScriptNodePtr pGroupNode = NULL;
	while( parentGroup.m_curGroupIndex < parentGroup.m_pScriptNode->m_childrenNodes.size() )
	{
		SScriptNodePtr pNode = parentGroup.m_pScriptNode->m_childrenNodes[ parentGroup.m_curGroupIndex ];
		if ( pNode->m_name == parentGroup.m_curGroupName )
		{
			pGroupNode = pNode;
			break;
		}
		else
		{
			++parentGroup.m_curGroupIndex; 
		}
	}

	if ( !pGroupNode )
	{
		return false;
	}
	else
	{
		SGroup group;
		group.m_curGroupIndex = 0;
		group.m_pScriptNode = pGroupNode;
		m_groupStack.push_back( group );
		return true;
	}
}

void CScriptNodeReader::EndGroup()
{
	if ( m_groupStack.size() > 1 )
	{
		m_groupStack.pop_back();
	}
}

bool CScriptNodeReader::Value( const String& name, int& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_Int )
	{
		val = (*attIt).m_int;
		return true;
	}

	return false;
}

bool CScriptNodeReader::Value( const String& name, float& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_Float )
	{
		val = (*attIt).m_float;
		return true;
	}

	return false;
}

bool CScriptNodeReader::Value( const String& name, String& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_String )
	{
		val = (*attIt).m_string;
		return true;
	}

	return false;
}

bool CScriptNodeReader::Value( const String& name, Vec2& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_Vector )
	{
		val.x = (*attIt).m_vector.x;
		val.y = (*attIt).m_vector.y;
		return true;
	}

	return false;
}

bool CScriptNodeReader::Value( const String& name, Vec3& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_Vector )
	{
		val.x = (*attIt).m_vector.x;
		val.y = (*attIt).m_vector.y;
		val.z = (*attIt).m_vector.z;
		return true;
	}

	return false;
}

bool CScriptNodeReader::Value( const String& name, Vec4& val )
{
	if ( m_groupStack.empty() )
	{
		return false;
	}

	SScriptNodePtr pNode = m_groupStack.back().m_pScriptNode;
	SScriptNode::TAttributeMap::FindRes attIt = pNode->m_attributes.FindElement( name );
	if ( attIt != nullptr && (*attIt).m_type == SScriptNode::SAttribute::eT_Vector )
	{
		val = (*attIt).m_vector;
		return true;
	}

	return false;
}





CScriptNodeWriter::CScriptNodeWriter()
{
	SScriptNodePtr pRootNode = new SScriptNode();
	pRootNode->m_name = "Root";
	m_nodeStack.push_back( pRootNode );
}

CScriptNodeWriter::~CScriptNodeWriter()
{
}

bool CScriptNodeWriter::IsReading() const
{
	return false;
}

bool CScriptNodeWriter::BeginGroup( const String& name )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNodePtr pChildNode = new SScriptNode();
	pChildNode->m_name = name;
	pNode->m_childrenNodes.push_back( pChildNode );
	
	m_nodeStack.push_back( pChildNode );
	
	return true;
}

void CScriptNodeWriter::EndGroup()
{
	if ( m_nodeStack.size() > 1 )
	{
		m_nodeStack.pop_back();
	}
}

bool CScriptNodeWriter::Value( const String& name, int& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_Int;
	attribute.m_int = val;
	pNode->m_attributes[ name ] = attribute;

	return true;
}

bool CScriptNodeWriter::Value( const String& name, float& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_Float;
	attribute.m_float = val;
	pNode->m_attributes[ name ] = attribute;

	return true;
}

bool CScriptNodeWriter::Value( const String& name, String& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_String;
	attribute.m_string = val;
	pNode->m_attributes[ name ] = attribute;
	
	return true;
}

bool CScriptNodeWriter::Value( const String& name, Vec2& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_Vector;
	attribute.m_vector = Vec4( val.x, val.y, 0.0f, 0.0f );
	pNode->m_attributes[ name ] = attribute;

	return true;
}

bool CScriptNodeWriter::Value( const String& name, Vec3& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_Vector;
	attribute.m_vector = Vec4( val.x, val.y, val.z, 0.0f );
	pNode->m_attributes[ name ] = attribute;

	return true;
}

bool CScriptNodeWriter::Value( const String& name, Vec4& val )
{
	if ( m_nodeStack.empty() )
	{
		return false;
	}
	
	SScriptNodePtr pNode = m_nodeStack.back();
	
	SScriptNode::SAttribute attribute;
	attribute.m_type = SScriptNode::SAttribute::eT_Vector;
	attribute.m_vector = val;
	pNode->m_attributes[ name ] = attribute;

	return true;
}

SScriptNodePtr	CScriptNodeWriter::GetRootNode()
{
	if ( m_nodeStack.empty() )
	{
		return NULL;
	}
	
	return m_nodeStack.back();
}

WHITEBOX_END