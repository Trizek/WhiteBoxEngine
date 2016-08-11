#include "Script.h"

WHITEBOX_BEGIN

#define CASE_SPACE	case ' ': \
case '\n': \
case '\t': \

#define CASE_DIGIT	case '0': \
case '1': \
case '2': \
case '3': \
case '4': \
case '5': \
case '6': \
case '7': \
case '8': \
case '9': \

#define CASE_LETTER	case 'a': \
case 'b': \
case 'c': \
case 'd': \
case 'e': \
case 'f': \
case 'g': \
case 'h': \
case 'i': \
case 'j': \
case 'k': \
case 'l': \
case 'm': \
case 'n': \
case 'o': \
case 'p': \
case 'q': \
case 'r': \
case 's': \
case 't': \
case 'u': \
case 'v': \
case 'w': \
case 'x': \
case 'y': \
case 'z': \
case 'A': \
case 'B': \
case 'C': \
case 'D': \
case 'E': \
case 'F': \
case 'G': \
case 'H': \
case 'I': \
case 'J': \
case 'K': \
case 'L': \
case 'M': \
case 'N': \
case 'O': \
case 'P': \
case 'Q': \
case 'R': \
case 'S': \
case 'T': \
case 'U': \
case 'V': \
case 'W': \
case 'X': \
case 'Y': \
case 'Z': \

#define CASE_SCORE	case '-': \
case '_': \


CScriptStreamReader::CScriptStreamReader()
	: m_anaLexState(eALS_None)
	, m_anaSynState(eASS_Node)
	, m_stringBufferLength(0)
	, m_intBuffer(0)
	, m_floatBuffer(0.0f)
	, m_curFloat(0)
	, m_floatDecimal(10)
{
	m_floatArrayBuffer[ 0 ] = 0.0f;
	m_floatArrayBuffer[ 1 ] = 0.0f;
	m_floatArrayBuffer[ 2 ] = 0.0f;
	m_floatArrayBuffer[ 3 ] = 0.0f;
}

SScriptNodePtr CScriptStreamReader::Parse( CDataStream& dataStream )
{
	SScriptNode* pRootNode = new SScriptNode();
	pRootNode->m_name = "Root";
	m_nodeStack.push_back( pRootNode );
	
	char character;
	while( dataStream.ReadByte( character ) )
	{
		ConsumeChar( character );
	}
	
	return pRootNode;
}


void CScriptStreamReader::ConsumeIdentifier()
{
	m_stringBuffer[m_stringBufferLength] = '\0';
	// printf("TokenIdentifier(%s)\n", m_stringBuffer);	
	String identifier( m_stringBuffer );

 	if ( m_anaSynState == eASS_Node )
 	{
		m_identifierBuffer = m_stringBuffer;
		m_anaSynState = eASS_NodeOrAttributeName;
 	}
 	else
 	{
 		// Fail
 	}	

	m_stringBufferLength = 0;
}

void CScriptStreamReader::ConsumeInt()
{
	//printf("TokenInt(%d)\n", m_intBuffer);
	
	if ( m_anaSynState == eASS_AttributeVal )
	{
		if ( !m_nodeStack.empty() )
		{
			SScriptNodePtr pCurNode = m_nodeStack.back();
			if ( pCurNode )
			{
				SScriptNode::SAttribute intAttribute;
				intAttribute.m_type = SScriptNode::SAttribute::eT_Int;
				intAttribute.m_int = m_intBuffer;
			
				// TODO: check there isnt already an attribute with that name
				pCurNode->m_attributes[ m_identifierBuffer ] = intAttribute;
			
				m_anaSynState = eASS_Node;
			}
		}
	}
	else if ( m_anaSynState == eASS_VectorVal )
	{
		m_floatArrayBuffer[ m_curFloat++ ] = (float)m_intBuffer;
	}		
	else
	{
		// Fail
	}
	
	m_intBuffer = 0;
}	

void CScriptStreamReader::ConsumeFloat()
{
	// printf("TokenFloat(%.5f)\n", m_floatBuffer);

	if ( m_anaSynState == eASS_AttributeVal )
	{
		if ( !m_nodeStack.empty() )
		{
			SScriptNodePtr pCurNode = m_nodeStack.back();
			if ( pCurNode )
			{
				SScriptNode::SAttribute floatAttribute;
				floatAttribute.m_type = SScriptNode::SAttribute::eT_Float;
				floatAttribute.m_float = m_floatBuffer;

				// TODO: check there isnt already an attribute with that name
				pCurNode->m_attributes[ m_identifierBuffer ] = floatAttribute;

				m_anaSynState = eASS_Node;
			}
		}
	}
	else if ( m_anaSynState == eASS_VectorVal )
	{
		m_floatArrayBuffer[ m_curFloat++ ] = m_floatBuffer;
	}	
	else
	{
		// Fail
	}

	m_floatBuffer = 0.0f;
	m_floatDecimal = 10;
}	

void CScriptStreamReader::ConsumeString()
{
	m_stringBuffer[m_stringBufferLength] = '\0';
	// printf("TokenString(%s)\n", m_stringBuffer);

	if ( m_anaSynState == eASS_AttributeVal )
	{
		if ( !m_nodeStack.empty() )
		{
			SScriptNodePtr pCurNode = m_nodeStack.back();
			if ( pCurNode )
			{
				SScriptNode::SAttribute stringAttribute;
				stringAttribute.m_type = SScriptNode::SAttribute::eT_String;
				stringAttribute.m_string = m_stringBuffer;

				// TODO: check there isnt already an attribute with that name
				pCurNode->m_attributes[ m_identifierBuffer ] = stringAttribute;

				m_anaSynState = eASS_Node;
			}
		}
	}
	else
	{
		// Fail
	}

	m_stringBufferLength = 0;
}	

void CScriptStreamReader::ConsumeOpenBracket()
{
	// printf("TokenOpenBracket\n");

	if ( m_anaSynState == eASS_NodeOrAttributeName )
	{
		if ( !m_nodeStack.empty() )
		{
			SScriptNodePtr pCurNode = m_nodeStack.back();
			if ( pCurNode )
			{
				SScriptNode* pChildNode = new SScriptNode();
				pChildNode->m_name = m_identifierBuffer;

				pCurNode->m_childrenNodes.push_back( pChildNode );

				m_nodeStack.push_back( pChildNode );

				m_anaSynState = eASS_Node;
			}
		}
	}
	else
	{
		// Fail
	}
}

void CScriptStreamReader::ConsumeCloseBracket()
{
	// printf("TokenCloseBracket\n");

	if ( m_anaSynState == eASS_Node )
	{
		if ( m_nodeStack.size() > 1 ) // Don't pop root node...
		{
			m_nodeStack.pop_back();
		}
	}
	else
	{
		// Fail
	}
}

void CScriptStreamReader::ConsumeOpenPar()
{
	if ( m_anaSynState == eASS_AttributeVal )
	{
		m_anaSynState = eASS_VectorVal;
		m_floatArrayBuffer[ 0 ] = 0.0f;
		m_floatArrayBuffer[ 1 ] = 0.0f;
		m_floatArrayBuffer[ 2 ] = 0.0f;
		m_floatArrayBuffer[ 3 ] = 0.0f;
		m_curFloat = 0;		
	}
	else
	{
		// Fail
	}
}

void CScriptStreamReader::ConsumeClosePar()
{
	if ( m_anaSynState == eASS_VectorVal )
	{
		// Consume vector
		if ( !m_nodeStack.empty() )
		{
			SScriptNodePtr pCurNode = m_nodeStack.back();
			if ( pCurNode )
			{
				SScriptNode::SAttribute vectorAttribute;
				vectorAttribute.m_type = SScriptNode::SAttribute::eT_Vector;
				vectorAttribute.m_vector.x = m_floatArrayBuffer[ 0 ];
				vectorAttribute.m_vector.y = m_floatArrayBuffer[ 1 ];
				vectorAttribute.m_vector.z = m_floatArrayBuffer[ 2 ];
				vectorAttribute.m_vector.w = m_floatArrayBuffer[ 3 ];
				
				m_floatArrayBuffer[ 0 ] = 0.0f;
				m_floatArrayBuffer[ 1 ] = 0.0f;
				m_floatArrayBuffer[ 2 ] = 0.0f;
				m_floatArrayBuffer[ 3 ] = 0.0f;
				m_curFloat = 0;

				// TODO: check there isnt already an attribute with that name
				pCurNode->m_attributes[ m_identifierBuffer ] = vectorAttribute;

				m_anaSynState = eASS_Node;
			}
		}		
	}
	else
	{
		// Fail
	}
}	

void CScriptStreamReader::ConsumeEqual()
{
	// printf("TokenEqualBracket\n");

	if ( m_anaSynState == eASS_NodeOrAttributeName )
	{
		m_anaSynState = eASS_AttributeVal;
	}
	else
	{
		// Fail
	}
}

void CScriptStreamReader::PushChar( char c )
{
	m_stringBuffer[ m_stringBufferLength++ ] = c;
}

void CScriptStreamReader::PushDigit( char digit )
{
	m_intBuffer = m_intBuffer * 10 + (digit - (int)'0');
}

void CScriptStreamReader::PushDecimal( char digit )
{
	m_floatBuffer += (digit - (int)'0') / float(m_floatDecimal);
	m_floatDecimal *= 10;
}	

void CScriptStreamReader::ConsumeChar( char c )
{
	switch( m_anaLexState )
	{
		case eALS_None:
		{
			switch( c )
			{
				CASE_LETTER
				{
					PushChar( c );
					m_anaLexState = eALS_Identifier;
				}
				break;

				CASE_DIGIT
				{
					PushDigit( c );
					m_anaLexState = eALS_Int;
				}

				break;

				case '\"':
				{
					m_anaLexState = eALS_String;
				}
				break;
				
				case '(':
				{
					ConsumeOpenPar();
					m_anaLexState = eALS_None;
				}
				break;
				
				case ')':
				{
					ConsumeClosePar();
					m_anaLexState = eALS_None;
				}
				break;				

				case '{':
				{
					ConsumeOpenBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '}':
				{
					ConsumeCloseBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '=':
				{
					ConsumeEqual();
					m_anaLexState = eALS_None;
				}
				break;	

				case ',':
				CASE_SPACE
				break;

				default:
				{
					// FAIL
				}
			}
		}

		break;

		case eALS_Identifier:
		{
			switch( c )
			{
				CASE_DIGIT
				CASE_LETTER
				CASE_SCORE
				{
					PushChar( c );
				}
				break;

				case '\"':
				{
					ConsumeIdentifier();
					m_anaLexState = eALS_String;
				}
				break;

				case '{':
				{
					ConsumeIdentifier();
					ConsumeOpenBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '}':
				{
					ConsumeIdentifier();
					ConsumeCloseBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '=':
				{
					ConsumeIdentifier();
					ConsumeEqual();
					m_anaLexState = eALS_None;
				}
				break;	

				CASE_SPACE
				{
					ConsumeIdentifier();
					m_anaLexState = eALS_None;
				}
				break;

				default:
				{
					// FAIL
				}	
			}
		}

		break;

		case eALS_String:
		{
			switch( c )
			{
				case '\"':
				{
					ConsumeString();
					m_anaLexState = eALS_None;
				}
				break;

				default:
				{
					PushChar( c );
				}
			}
		}
		break;	

		case eALS_Int:
		{
			switch( c )
			{
				CASE_LETTER
				{
					ConsumeInt();
					PushChar( c );
					m_anaLexState = eALS_Identifier;
				}
				break;

				CASE_DIGIT
				{
					PushDigit( c );
				}
				break;

				case '\"':
				{
					ConsumeInt();
					m_anaLexState = eALS_String;
				}
				break;
				
				case ')':
				{
					ConsumeInt();
					ConsumeClosePar();
					m_anaLexState = eALS_None;
				}
				break;					

				case '{':
				{
					ConsumeInt();
					ConsumeOpenBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '}':
				{
					ConsumeInt();
					ConsumeCloseBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '=':
				{
					ConsumeInt();
					ConsumeEqual();
					m_anaLexState = eALS_None;
				}
				break;	

				case '.':
				{
					m_floatBuffer = (float)m_intBuffer;
					m_intBuffer = 0;
					m_anaLexState = eALS_Float;
				}
				break;

				case ',':
				CASE_SPACE
				{
					ConsumeInt();
					m_anaLexState = eALS_None;
				}
				break;

				default:
				{
					// FAIL
				}
			}
		}
		break;

		case eALS_Float:
		{
			switch( c )
			{
				CASE_LETTER
				{
					ConsumeFloat();
					PushChar( c );
					m_anaLexState = eALS_Identifier;
				}
				break;

				CASE_DIGIT
				{
					PushDecimal( c );
				}
				break;

				case '\"':
				{
					ConsumeFloat();
					m_anaLexState = eALS_String;
				}
				break;
				
				case ')':
				{
					ConsumeFloat();
					ConsumeClosePar();
					m_anaLexState = eALS_None;
				}
				break;					

				case '{':
				{
					ConsumeFloat();
					ConsumeOpenBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '}':
				{
					ConsumeInt();
					ConsumeCloseBracket();
					m_anaLexState = eALS_None;
				}
				break;

				case '=':
				{
					ConsumeFloat();
					ConsumeEqual();
					m_anaLexState = eALS_None;
				}
				break;	

				case ',':
				CASE_SPACE
				{
					ConsumeFloat();
					m_anaLexState = eALS_None;
				}
				break;

				default:
				{
					// FAIL
				}
			}
		}
		break;
	}
}



CScriptFileWriter::CScriptFileWriter( const char* filePath )
{
	m_pFile = nullptr;
#if defined(_WIN32) || defined(_WIN64)
	fopen_s( &m_pFile, filePath, "w");
#else
	m_pFile = fopen( filePath, "w" );
#endif
	m_indentation = 0;
}

CScriptFileWriter::~CScriptFileWriter()
{
	fclose( m_pFile );
}
	
void CScriptFileWriter::WriteNode( const SScriptNodePtr& pNode )
{
	// Write attributes
	for( SScriptNode::TAttributeMap::const_iterator attIt = pNode->m_attributes.begin() ; attIt != pNode->m_attributes.end() ; ++attIt )
	{
		NewLine();
	
		const SScriptNode::SAttribute& attribute = attIt->second;
		switch( attribute.m_type )
		{
			case SScriptNode::SAttribute::eT_Int:
			{
				fprintf( m_pFile, "%s = %d", attIt->first.c_str(), attribute.m_int );
			}
			break;
			
			case SScriptNode::SAttribute::eT_Float:
			{
				fprintf( m_pFile, "%s = %.5f", attIt->first.c_str(), attribute.m_float );
			}
			break;
			
			case SScriptNode::SAttribute::eT_String:
			{
				fprintf( m_pFile, "%s = \"%s\"", attIt->first.c_str(), attribute.m_string.c_str() );
			}
			break;			
			
			case SScriptNode::SAttribute::eT_Vector:
			{
				fprintf( m_pFile, "%s = ", attIt->first.c_str() );
				if ( attribute.m_vector.w == 0.0f )
				{
					if ( attribute.m_vector.z == 0.0f )
					{
						if ( attribute.m_vector.y == 0.0f )
						{
							if ( attribute.m_vector.x == 0.0f )
							{
								fprintf( m_pFile, "()" );
							}
							else
							{
								fprintf( m_pFile, "(%.5f)", attribute.m_vector.x );
							}
						}
						else
						{
							fprintf( m_pFile, "(%.5f, %.5f)", attribute.m_vector.x, attribute.m_vector.y );
						}
					}
					else
					{
						fprintf( m_pFile, "(%.5f, %.5f, %.5f)", attribute.m_vector.x, attribute.m_vector.y, attribute.m_vector.z );
					}
				}
				else
				{
					fprintf( m_pFile, "(%.5f, %.5f, %.5f, %.5f)", attribute.m_vector.x, attribute.m_vector.y, attribute.m_vector.z, attribute.m_vector.w );
				}
			}
			break;																
		}
	}
	
	
	// Write child nodes
	for( size_t iChild = 0 ; iChild < pNode->m_childrenNodes.size() ; ++iChild )
	{
		NewLine();
		NewLine();
		SScriptNodePtr pChildNode = pNode->m_childrenNodes[ iChild ];
		fprintf( m_pFile, pChildNode->m_name.c_str() );
		
		NewLine();
		fprintf( m_pFile, "{" );
		
		++m_indentation;
		WriteNode( pChildNode );
		--m_indentation;
		
		NewLine();
		fprintf( m_pFile, "}" );		
	}
}
	
void CScriptFileWriter::NewLine()
{
	fprintf( m_pFile, "\n" );
	for( int i=0 ; i < m_indentation ; ++i )
	{
		fprintf( m_pFile, "  " );
	}
}


WHITEBOX_END
