#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "BaseTypes.h"
#include "DataStream.h"
#include "Tokenizer.h"

WHITEBOX_BEGIN

struct SScriptNode : public CSmartPointerData
{
	struct SAttribute
	{
		enum EType
		{
			eT_String = 0,
			eT_Int,
			eT_Float,
			eT_Vector,
		};

		SAttribute()
			: m_type(eT_String)
			, m_int(0){}

		EType m_type;
		String m_string;
		Vec4 m_vector;

		union
		{
			int	m_int;
			float m_float;
		};
	};

	String m_name;	

	std::vector< CSmartPointer< SScriptNode > > m_childrenNodes;
	
	typedef Map< String, SAttribute > TAttributeMap;
	TAttributeMap m_attributes;
};

DEFINE_SMART_PTR( SScriptNode )


class CScriptStreamReader
{
public:
	CScriptStreamReader();
	
	SScriptNodePtr Parse( CDataStream& dataStream );

private:
	bool	ConsumeInstruction();
	
	bool	ConsumeScope();
	bool	ConsumeAttribute();

	// Syntaxic analysis buffer
	String							m_identifierBuffer;
	std::vector< SScriptNodePtr >	m_nodeStack;
	CTokenizer						m_tokenizer;
};

class CScriptFileWriter
{
public:
	CScriptFileWriter( const char* filePath );
	~CScriptFileWriter();
	
	void WriteNode( const SScriptNodePtr& pNode );
	
private:
	void NewLine();

	int	m_indentation;
	FILE* m_pFile;
};

WHITEBOX_END

#endif
