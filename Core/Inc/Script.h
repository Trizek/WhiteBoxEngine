#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "BaseTypes.h"
#include "DataStream.h"

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
	
	typedef typename Map< String, SAttribute > TAttributeMap;
	TAttributeMap m_attributes;
};

DEFINE_SMART_PTR( SScriptNode )


class CScriptStreamReader
{
public:
	CScriptStreamReader();
	
	SScriptNodePtr Parse( CDataStream& dataStream );

private:
	// Lexical analysis
	void ConsumeIdentifier();
	void ConsumeInt();
	void ConsumeFloat();
	void ConsumeVector();
	void ConsumeString();
	void ConsumeOpenBracket();
	void ConsumeCloseBracket();
	void ConsumeOpenPar();
	void ConsumeClosePar();	
	void ConsumeEqual();

	// Lexical analysis bufferization
	void PushChar( char c );
	void PushDigit( char digit );
	void PushDecimal( char digit );

	// Consume char in the lexical analyser
	void ConsumeChar( char c );

	enum EAnaLexState
	{
		eALS_None = 0,
		eALS_Identifier,
		eALS_String,
		eALS_Int,
		eALS_Float,
	};

	enum EAnaSynState
	{
		eASS_Node = 0,
		eASS_NodeOrAttributeName,
		eASS_AttributeVal,
		eASS_VectorVal,
	};

	static const int STRING_BUFFER_SIZE = 512;

	EAnaLexState m_anaLexState;
	EAnaSynState m_anaSynState;

	// Lexical analysis buffer
	char	m_stringBuffer[ STRING_BUFFER_SIZE ];
	int		m_stringBufferLength;

	int		m_intBuffer;
	float	m_floatBuffer;
	float	m_floatArrayBuffer[ 4 ];
	size_t	m_curFloat;
	int		m_floatDecimal;

	// Syntaxic analysis buffer
	String	m_identifierBuffer;

	std::vector< SScriptNodePtr > m_nodeStack;
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
