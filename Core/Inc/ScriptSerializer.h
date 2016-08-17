#ifndef __SCRIPTSERIALIZER_H__
#define __SCRIPTSERIALIZER_H__

#include "Serializer.h"
#include "Script.h"

WHITEBOX_BEGIN

class CScriptNodeReader : public ISerializer
{
public:
	CScriptNodeReader( SScriptNodePtr pRootNode );

	// ISerializer
	virtual bool IsReading() const;
	virtual bool BeginGroup( const String& name );
	virtual void EndGroup();
	virtual bool Value( const String& name, int& val );
	virtual bool Value( const String& name, float& val );
	virtual bool Value( const String& name, String& val );
	virtual bool Value( const String& name, Vec2& val );
	virtual bool Value( const String& name, Vec3& val );
	virtual bool Value( const String& name, Vec4& val );
	// ~ISerializer

private:
	struct SGroup
	{
		SScriptNodePtr m_pScriptNode;
		String	m_curGroupName;
		size_t	m_curGroupIndex;
	};

	std::vector< SGroup > m_groupStack;
};

class CScriptNodeWriter : public ISerializer
{
public:
	CScriptNodeWriter();
	~CScriptNodeWriter();

	// ISerializer
	virtual bool IsReading() const;
	virtual bool BeginGroup( const String& name );
	virtual void EndGroup();
	virtual bool Value( const String& name, int& val );
	virtual bool Value( const String& name, float& val );
	virtual bool Value( const String& name, String& val );
	virtual bool Value( const String& name, Vec2& val );
	virtual bool Value( const String& name, Vec3& val );
	virtual bool Value( const String& name, Vec4& val );
	// ~ISerializer
	
	SScriptNodePtr	GetRootNode();

private:
	std::vector< SScriptNodePtr > m_nodeStack;
};

WHITEBOX_END

#endif
