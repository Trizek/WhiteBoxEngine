

#include "Handle.h"
#include "Script.h"


struct ISerializer
{
	virtual bool IsReading() const = 0;

	virtual bool BeginGroup( const Ogre::String& name ) = 0;
	virtual void EndGroup() = 0;

	virtual void Value( const Ogre::String& name, int& val ) = 0;
	virtual void Value( const Ogre::String& name, float& val ) = 0;
	virtual void Value( const Ogre::String& name, Ogre::String& val ) = 0;
};

class CScriptReader
{
	CScriptReader()
	{
		ScriptNode* pRootNode = m_scriptParser.Parse();
		if ( pRootNode )
		{
			SGroup group;
			group.m_pScriptNode = pRootNode;
			group.m_curGroupIndex = 0;
			m_groupStack.push_back( group );
		}
	}

	virtual bool IsReading() const
	{
		return true;
	}

	virtual bool BeginGroup( const Ogre::String& name )
	{
		if ( m_groupStack.empty() )
		{
			return false;
		}

		SGroup& parentGroup = m_groupStack.front();

		if ( parentGroup.m_curGroupName != name )
		{
			parentGroup.m_curGroupName = name;
			parentGroup.m_curGroupIndex = 0;
		}
		else
		{
			++parentGroup.m_curGroupIndex;
		}

		ScriptNode* pGroupNode = NULL;
		while( parentGroup.m_curGroupIndex < parentGroup.m_pScriptNode->m_childrenNodes.size() )
		{
			ScriptNode* pNode = parentGroup.m_pScriptNode->m_childrenNodes[ parentGroup.m_curGroupIndex ];
			if ( pNode->m_name == parentGroup.m_curGroupName )
			{
				pGroupNode = pNode;
			}
			else
			{
				++parentGroup.m_curGroupIndex; 
			}
		}

		if ( pGroupNode == NULL )
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

	virtual void EndGroup()
	{
		if ( m_groupStack.size() > 1 )
		{
			m_groupStack.pop_back();
		}
	}

	virtual void Value( const Ogre::String& name, int& val )
	{
		if ( m_groupStack.empty() )
		{
			return;
		}
 
 		ScriptNode* pNode = m_groupStack.back().m_pScriptNode;
		std::hash_map< Ogre::String, ScriptNode::SAttribute >::iterator attIt = pNode->m_attributes.find( name );
		if ( attIt != pNode->m_attributes.end() && attIt->second.m_type == ScriptNode::SAttribute::eT_Int )
		{
			val = attIt->second.m_int;
		}
	}

	virtual void Value( const Ogre::String& name, float& val )
	{
		if ( m_groupStack.empty() )
		{
			return;
		}

		ScriptNode* pNode = m_groupStack.back().m_pScriptNode;
		std::hash_map< Ogre::String, ScriptNode::SAttribute >::iterator attIt = pNode->m_attributes.find( name );
		if ( attIt != pNode->m_attributes.end() && attIt->second.m_type == ScriptNode::SAttribute::eT_Float )
		{
			val = attIt->second.m_float;
		}
	}

	virtual void Value( const Ogre::String& name, Ogre::String& val )
	{
		if ( m_groupStack.empty() )
		{
			return;
		}

		ScriptNode* pNode = m_groupStack.back().m_pScriptNode;
		std::hash_map< Ogre::String, ScriptNode::SAttribute >::iterator attIt = pNode->m_attributes.find( name );
		if ( attIt != pNode->m_attributes.end() && attIt->second.m_type == ScriptNode::SAttribute::eT_String )
		{
			val = attIt->second.m_string;
		}
	}

	struct SGroup
	{
		ScriptNode* m_pScriptNode;
		Ogre::String	m_curGroupName;
		size_t	m_curGroupIndex;

	};

	ScriptParser m_scriptParser;
	std::vector< SGroup > m_groupStack;
};



class CObject;

enum EComponent
{
	eC_Entity = 0,
	eC_Count,
};


typedef int ObjectId;

class CObjectManager
{
public:
	CObject*	GetOrCreateObject( ObjectId id );
	CObject*	GetObject( ObjectId id )
	{
		return m_pObjectMap[id];
	}


private:
	ObjectId m_nextId;
	std::hash_map< ObjectId, CObject* > m_pObjectMap;
};

CObjectManager* gObjectManager;

class CComponent
{
public:
	CComponent() : m_pOwner(NULL){}

	virtual void Serialize( ISerializer& serializer ) = 0;

	CObject&	GetOwner()
	{
		return *m_pOwner;
	}

	void SetOwner( CObject& object )
	{
		m_pOwner = &object;
	}

private:
	CObject*	m_pOwner;
};


class CObject
{
public:
	CObject( ObjectId id )
		: m_id(id) 
		, m_pParentObject(NULL)
		,	m_pSceneNode(NULL)
	{
		for( size_t iComp=0 ; iComp < eC_Count ; ++iComp )
		{
			m_pComponents[iComp] = NULL;
		}
	}

	void AttachChild( CObject& object )
	{
		// remove previous parentship
		if ( object.m_pParentObject )
		{
			object.m_pParentObject->DetachChild( object );
			object.m_pParentObject = NULL;
		}

		// attach as child
		m_pChildrenObjectMap[ object.m_id ] = &object;

		// set parent
		object.m_pParentObject = this;
	}

	void DetachChild( CObject& object )
	{
//		std::remove( m_pChildrenObjectMap.begin(), m_pChildrenObjectMap.end(), object.m_id );
	}

	void Serialize( ISerializer& serializer )
	{
		if ( serializer.BeginGroup( "Children") )
		{
			int childrenCount = (int)m_pChildrenObjectMap.size();
			serializer.Value( "count", childrenCount );

			if ( serializer.IsReading() )
			{
				for( int iChild=0 ; iChild < childrenCount ; ++iChild )
				{
					if ( !serializer.BeginGroup( "Object" ) )
					{
						continue;
					}

					ObjectId childId = 0;
					serializer.Value( "id", childId );

					if ( childId )
					{
						CObject*& pChildObject = m_pChildrenObjectMap[childId];
						if ( pChildObject == NULL )
						{
							// Find (or create) object and attach it
							pChildObject = gObjectManager->GetOrCreateObject( childId );
							AttachChild( *pChildObject );
						}
					}

					serializer.EndGroup();
				}
			}
			else // writing
			{
				std::hash_map< ObjectId, CObject* >::iterator childIt;
				for( childIt = m_pChildrenObjectMap.begin() ; childIt != m_pChildrenObjectMap.end() ; ++childIt )
				{
					serializer.BeginGroup( "Object" );

					CObject* pObject = childIt->second;
					if ( pObject )
					{
						pObject->Serialize( serializer );
					}

					serializer.EndGroup();
				}
			}
		}
		serializer.EndGroup();
	}


private:
	ObjectId		m_id;
	CObject*		m_pParentObject;
	std::hash_map< ObjectId, CObject* >	m_pChildrenObjectMap;
	CComponent*	m_pComponents[eC_Count];

	Ogre::SceneNode*	m_pSceneNode;
};



class CEntityComponent : public CComponent
{
public:
	virtual void Serialize( ISerializer& serializer )
	{
		serializer.Value( "entName", m_entName );
		serializer.Value( "meshName", m_meshName );
	}

private:
	Ogre::String	m_entName;
	Ogre::String	m_meshName;
};

