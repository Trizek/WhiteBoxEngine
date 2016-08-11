#ifndef __TEXT_H__
#define __TEXT_H__


#include "WhiteBoxNameSpace.h"

#include <vector>

WHITEBOX_BEGIN

class CText
{
public:
	CText() = default;

 	explicit CText( const String& string )
 	{
 		m_str.resize( string.length() );
 		for ( uint i = 0; i < string.length(); ++i )
 		{
 			m_str[i] = string.c_str()[i];
 		}
 	}

	CText( const wchar* unicodeText )
	{
		uint length = 0;
		{
			const wchar* buffer = unicodeText;
			while ( *buffer != 0 )
			{
				++buffer; ++length;
			}
		}

		m_str.resize( length );

#if defined( _WIN64 ) || defined( _WIN32 )
		memcpy_s( &m_str[0], length * sizeof(wchar), unicodeText, length * sizeof(wchar) );
#else
		memcpy( &m_str[0], unicodeText, length * sizeof(wchar) );
#endif
	}

	const wchar operator[]( size_t index ) const
	{
		return m_str[ index ];
	}

	wchar& operator[]( size_t index )
	{
		return m_str[ index ];
	}

	const wchar*	c_str() const
	{
		return ( m_str.empty() )? nullptr : &m_str[0];
	}

	size_t	length() const
	{
		return m_str.size();
	}

private:
	std::vector< wchar >	m_str;
};

WHITEBOX_END

#endif


 
