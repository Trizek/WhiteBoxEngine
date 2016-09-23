#include "Script.h"
#include "Tokenizer.h"

WHITEBOX_BEGIN

CScriptStreamReader::CScriptStreamReader()
{}

SScriptNodePtr CScriptStreamReader::Parse( CDataStream& dataStream )
{
	SScriptNode* pRootNode = new SScriptNode();
	pRootNode->m_name = "Root";
	m_nodeStack.push_back( pRootNode );

	m_tokenizer.SetDataStream( dataStream );

	while ( ConsumeInstruction() );

	return pRootNode;
}


bool	CScriptStreamReader::ConsumeInstruction()
{
	if ( !m_tokenizer.ReadToken() )
	{
		return false;
	}

	if ( m_tokenizer.GetTokenType() != (int)ETokenType::eIdentifier )
	{
		// Expected identifier
		return false;
	}
	m_identifierBuffer = m_tokenizer.GetString();
	
	if ( !m_tokenizer.ReadToken() )
	{
		return false;
	}

	if ( ConsumeScope() )
	{
		return true;
	}
	else if ( ConsumeAttribute() )
	{
		return true;
	}

	return false;
}

bool	CScriptStreamReader::ConsumeScope()
{
	if ( m_nodeStack.empty() || !m_nodeStack.back() )
	{
		// error
		return false;
	}

	if ( m_tokenizer.GetTokenType() != '{' )
	{
		return false;
	}

	SScriptNodePtr pCurNode = m_nodeStack.back();
	
	SScriptNode* pChildNode = new SScriptNode();
	pChildNode->m_name = m_identifierBuffer;
	pCurNode->m_childrenNodes.push_back( pChildNode );
	m_nodeStack.push_back( pChildNode );

	while( ConsumeInstruction() );

	if ( m_tokenizer.GetTokenType() != '}' )
	{
		// error expected }
		return false;
	}

	if ( m_nodeStack.size() > 1 ) // Don't pop root node...
	{
		m_nodeStack.pop_back();
	}

	return true;
}

bool	CScriptStreamReader::ConsumeAttribute()
{
	SScriptNodePtr pCurNode;
	if ( m_nodeStack.empty() || !m_nodeStack.back() )
	{
		// error
		return false;
	}

	if ( m_tokenizer.GetTokenType() != '=' )
	{
		return false;
	}

	if ( !m_tokenizer.ReadToken() )
	{
		return false;
	}

	pCurNode = m_nodeStack.back();
	if ( m_tokenizer.GetTokenType() == (int)ETokenType::eInt )
	{
		SScriptNode::SAttribute intAttribute;
		intAttribute.m_type = SScriptNode::SAttribute::eT_Int;
		intAttribute.m_int = m_tokenizer.GetInt();

		pCurNode->m_attributes[ m_identifierBuffer ] = intAttribute;

		return true;
	}
	else if ( m_tokenizer.GetTokenType() == (int)ETokenType::eFloat )
	{
		SScriptNode::SAttribute floatAttribute;
		floatAttribute.m_type = SScriptNode::SAttribute::eT_Float;
		floatAttribute.m_float = m_tokenizer.GetFloat();

		pCurNode->m_attributes[ m_identifierBuffer ] = floatAttribute;

		return true;
	}
	else if  ( m_tokenizer.GetTokenType() == (int)ETokenType::eString )
	{
		SScriptNode::SAttribute stringAttribute;
		stringAttribute.m_type = SScriptNode::SAttribute::eT_String;
		stringAttribute.m_string = m_tokenizer.GetString();

		pCurNode->m_attributes[ m_identifierBuffer ] = stringAttribute;

		return true;
	}

	return false;
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


 	for( SScriptNode::TAttributeMap::ConstIterator attIt = pNode->m_attributes.GetConstIterator() ; attIt.IsValid() ; attIt.MoveNext() )
 	{
 		NewLine();
 	
 		const SScriptNode::SAttribute& attribute = attIt.GetValue();
 		switch( attribute.m_type )
 		{
 			case SScriptNode::SAttribute::eT_Int:
 			{
 				fprintf( m_pFile, "%s = %d", attIt.GetKey().c_str(), attribute.m_int );
 			}
 			break;
 			
 			case SScriptNode::SAttribute::eT_Float:
 			{
 				fprintf( m_pFile, "%s = %.5f", attIt.GetKey().c_str(), attribute.m_float );
 			}
 			break;
 			
 			case SScriptNode::SAttribute::eT_String:
 			{
 				fprintf( m_pFile, "%s = \"%s\"", attIt.GetKey().c_str(), attribute.m_string.c_str() );
 			}
 			break;			
 			
 			case SScriptNode::SAttribute::eT_Vector:
 			{
				break;

 				fprintf( m_pFile, "%s = ", attIt.GetKey().c_str() );
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
		fputs( pChildNode->m_name.c_str(), m_pFile );
		
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
