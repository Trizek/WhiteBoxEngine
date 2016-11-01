#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "BaseTypes.h"
#include "DataStream.h"

WHITEBOX_BEGIN

enum class ETokenType
{
	eString = 1 << sizeof(char) * 8,
	eInt,
	eFloat,
	eIdentifier,
};

class CTokenizer
{
public:
	CTokenizer();

	void	SetDataStream( const CDataStream& dataStream );
	void	AddKeyword( const String& name, int val );

	bool	ReadToken();
	void	SkipLine();

	int		GetTokenType() const;
	int		GetInt() const;
	float	GetFloat() const;
	String	GetString() const;
	int		GetKeyword() const;

	void	PrintTokens();

private:

	static bool IsLetter( char c );
	static bool IsDigit( char c );

	static bool	IsIdentifierCharacter( char c );
	static bool	IsWhiteSpace( char c );

	void		SkipWhiteSpaces();
	bool		ReadOperators( char firstChar );

	bool		ReadNumber( char firstChar );

	bool		ReadString( char firstChar );

	bool		ReadIdentifier( char firstChar );


	typedef bool (CTokenizer::*TTokenFunc)(char);

	int							m_int;
	float						m_float;
	String						m_string;
	int							m_keyword;

	int							m_tokenType;
	CDataStream					m_dataStream;

	HashMap< String, int >		m_keywords;
	std::vector< TTokenFunc >	m_tokenFunctions;
};

WHITEBOX_END

#endif