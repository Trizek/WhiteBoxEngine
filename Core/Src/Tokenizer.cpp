#include "Tokenizer.h"

WHITEBOX_BEGIN

CTokenizer::CTokenizer()
	: m_tokenType(-1)
{
	m_tokenFunctions.push_back( &CTokenizer::ReadOperators );
	m_tokenFunctions.push_back( &CTokenizer::ReadNumber );
	m_tokenFunctions.push_back( &CTokenizer::ReadString );
	m_tokenFunctions.push_back( &CTokenizer::ReadIdentifier );
}


void	CTokenizer::SetDataStream( const CDataStream& dataStream )
{
	m_dataStream = dataStream;
}

void	CTokenizer::AddKeyword( const String& name, int val )
{
	m_keywords.AddPair( name, val );
}

bool	CTokenizer::ReadToken()
{
	m_keyword = -1;

	SkipWhiteSpaces();

	if ( m_dataStream.EndOfStream() )
	{
		m_tokenType = -1;
		return false;
	}

	for( TTokenFunc& func : m_tokenFunctions )
	{
		if ( (this->*func)(m_dataStream.Get()) )
		{
			return true;
		}
	}

	m_tokenType = -1;
	WbLog( "Tokenizer", "Invalid token" );

	return false;
}

void	CTokenizer::SkipLine()
{
	while ( !m_dataStream.EndOfStream() && m_dataStream.Get() != '\n' )
	{
		++m_dataStream;
	}
	++m_dataStream;
}

int		CTokenizer::GetTokenType() const
{
	return m_tokenType;
}

int		CTokenizer::GetInt() const
{
	return m_int;
}

float	CTokenizer::GetFloat() const
{
	return m_float;
}

String	CTokenizer::GetString() const
{
	return m_string;
}

int		CTokenizer::GetKeyword() const
{
	return m_keyword;
}

void	CTokenizer::PrintTokens()
{
	m_dataStream.SetCursor( 0 );

	while ( ReadToken() )
	{
		switch (m_tokenType)
		{
		case ETokenType::eInt:			printf("Int: %d\n", m_int); break;
		case ETokenType::eFloat:		printf("Float: %.4f\n", m_float); break;
		case ETokenType::eString:		printf("String: %s\n", m_string.c_str()); break;
		case ETokenType::eIdentifier:	if (m_keyword < 0) { printf("Identifier: %s\n", m_string.c_str()); }else { printf("Keyword: %s %d\n", m_string.c_str(), m_keyword); } break;
		default: printf("Operator %c\n", (char)m_tokenType); break;
		}
	}

	m_dataStream.SetCursor( 0 );
}

bool	CTokenizer::IsLetter( char c )
{
	return ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') );
}

bool	CTokenizer::IsDigit( char c )
{
	return ( c >= '0' && c <= '9' );
}

bool	CTokenizer::IsIdentifierCharacter( char c )
{
	return ( c == '_' || IsLetter(c) || IsDigit(c) );
}

bool	CTokenizer::IsWhiteSpace( char c )
{
	return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' );
}

void	CTokenizer::SkipWhiteSpaces()
{
	while ( !m_dataStream.EndOfStream() && IsWhiteSpace(m_dataStream.Get()) )
	{
		++m_dataStream;
	}
}

bool	CTokenizer::ReadOperators( char firstChar )
{
	switch( firstChar )
	{
		case '(':
		case ')':
		case '{':
		case '}':
		case '=':
		case '#':
		case '/':
		case '\\':
		{
			++m_dataStream;
			m_tokenType = firstChar;
			return true;
		}
		default:
			return false;
	}
}

bool	CTokenizer::ReadNumber( char firstChar )
{
	char const* curPtr = (char const*)m_dataStream.GetCursorData();
	char* endIntPtr = nullptr;
	char* endFloatPtr = nullptr;
		
	long intNumber = strtol( curPtr, &endIntPtr, 10 );
	float floatNumber = strtof( curPtr, &endFloatPtr );
		
	if ( curPtr == endIntPtr && curPtr == endFloatPtr )
	{
		return false;
	}

	size_t offset;
	if ( endFloatPtr > endIntPtr )
	{
		m_tokenType = (int)ETokenType::eFloat;
		m_float = floatNumber;
		offset = endFloatPtr - curPtr;
	}
	else
	{
		m_tokenType = (int)ETokenType::eInt;
		m_int = (int)intNumber;
		offset = endIntPtr - curPtr;
	}
		
		
	m_dataStream += offset;

	return true;
}

bool	CTokenizer::ReadString( char firstChar )
{
	if ( firstChar != '"' )
	{
		return false;
	}

	char buffer[ 512 ];
	size_t bufferLen = 0;

	++m_dataStream;
		
	for ( ; !m_dataStream.EndOfStream() && m_dataStream.Get() != '"'; ++m_dataStream )
	{
		char c = m_dataStream.Get();
		if ( c == '\\' )
		{
			++m_dataStream;
			if ( m_dataStream.EndOfStream() )
			{
				break;
			}
		}

		buffer[ bufferLen++ ] = m_dataStream.Get();
	}
	buffer[ bufferLen ] = '\0';

	m_string = String( buffer );
	m_tokenType = (int)ETokenType::eString;
	++m_dataStream;
		
	return true;
}

bool	CTokenizer::ReadIdentifier( char firstChar )
{
	if ( firstChar != '_' && !IsLetter(firstChar) )
	{
		return false;
	}

	char const* str = (char const*)m_dataStream.GetCursorData();
		
	for( ; !m_dataStream.EndOfStream() && IsIdentifierCharacter(m_dataStream.Get()); ++m_dataStream );

	size_t len = (char const*)m_dataStream.GetCursorData() - str;

	m_string = String( str, len );
	m_tokenType = (int)ETokenType::eIdentifier;


	for( auto it = m_keywords.GetIterator(); it.IsValid(); it.MoveNext() )
	{
		const String& key = it.GetKey();
		int val = it.GetValue();

		if ( key == m_string )
		{
			m_keyword = val;
			return true;
		}
	}

	return true;
}


WHITEBOX_END