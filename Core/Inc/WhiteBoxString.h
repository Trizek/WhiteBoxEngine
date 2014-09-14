#ifndef __WHITEBOXSTRING_H__
#define __WHITEBOXSTRING_H__

#include <stdio.h>
#include <string.h>

#include "WhiteBoxNameSpace.h"


WHITEBOX_BEGIN

class CString
{
public:
	static const size_t npos = -1;

	CString()
	: m_str(NULL)
	, m_length(0)
	, m_pRefCount(NULL)
	, m_hash(0)
	{}
		
	CString( const CString& rhs )
	{
		Copy( rhs );
	}
		
	CString( const char* str )
	{
		Set( str );
	}
	
	CString( const char* str, size_t length )
	{
		SetTruncate( str, length );
	}
			
		
	~CString()
	{
		DecreaseRefCount();
	}
		
	CString& operator=( const CString& rhs )
	{
		DecreaseRefCount();
		Copy( rhs );

		return *this;
	}
		
	bool operator==( const CString& rhs ) const
	{
		return strcmp( m_str, rhs.m_str ) == 0; //m_hash == rhs.m_hash;
	}
		
	bool operator!=( const CString& rhs ) const
	{
		return m_hash != rhs.m_hash;
	}
		
	bool operator<( const CString& rhs ) const
	{
		return m_hash < rhs.m_hash;
	}

	bool operator<=( const CString& rhs ) const
	{
		return m_hash <= rhs.m_hash;
	}

	bool operator>( const CString& rhs ) const
	{
		return m_hash > rhs.m_hash;
	}

	bool operator>=( const CString& rhs ) const
	{
		return m_hash >= rhs.m_hash;
	}
		
	const char* c_str() const
	{
		return m_str;
	}
	
	bool	empty() const
	{
		return m_length == 0;
	}

	size_t	size() const
	{
		return m_length;
	}
		
	size_t	length() const
	{
		return m_length;
	}

	size_t	getHash() const
	{
		return m_hash;
	}
	
	size_t find (char c, size_t pos = 0) const
	{
		for( size_t iChar = pos ; iChar < m_length ; ++iChar )
		{
			if ( m_str[ iChar ] == c )
			{
				return iChar;
			}
		}
		
		return npos;
	}
	
	CString substr (size_t pos = 0, size_t len = npos) const
	{
		size_t offset = pos;
		if (offset > m_length)
		{
			offset = m_length;
		}
		
		if ( len == npos || len > m_length - offset )
		{
			return CString( m_str + offset );
		}
		else
		{
			return CString( m_str + offset, len );
		}
	}
	
	CString get_path_extension() const
	{
		size_t point_pos = find( '.' );
		if ( point_pos == npos )
		{
			return CString();
		}
		
		return substr( point_pos + 1 ); 
	}
	
	CString get_path_name() const
	{
		size_t point_pos = find( '.' );
		if ( point_pos == npos || point_pos == 0 )
		{
			return CString();
		}	
	
		size_t name_pos = point_pos - 1;
		while( name_pos > 0 && ( c_str()[ name_pos - 1 ] != '/' ) )
		{
			--name_pos;
		}
		
		return substr( name_pos, point_pos - name_pos );	
	}
	
	CString get_path_base() const
	{
		size_t point_pos = find( '.' );
		if ( point_pos == npos || point_pos == 0 )
		{
			return CString();
		}	
	
		size_t name_pos = point_pos - 1;
		while( name_pos > 0 && ( c_str()[ name_pos - 1 ] != '/' ) )
		{
			--name_pos;
		}
		
		return substr( 0, name_pos );	
	}	

	CString operator+( const CString& rhs ) const
	{
		CString res;
	
		res.m_length = m_length + rhs.m_length;
		res.Allocate();
		if ( m_str != NULL )
		{
			strcpy( res.m_str, m_str );
		}
		if ( rhs.m_str != NULL )
		{
			strcpy( res.m_str + m_length, rhs.m_str );
		}
		res.m_str[ res.m_length ] = '\0';
		res.ComputeHash();
	
		return res;
	}
	
private:
	void	DecreaseRefCount()
	{
		if ( m_pRefCount && m_str )
		{
			--(*m_pRefCount);
			if ( *m_pRefCount == 0 )
			{
				delete m_pRefCount;
				delete[] m_str;
			}
		}	
	}
		
	void	Copy( const CString& rhs )
	{
		m_str = rhs.m_str;
		m_length = rhs.m_length;
		m_pRefCount = rhs.m_pRefCount;
		m_hash = rhs.m_hash;
			
		if ( m_pRefCount )
		{
			++(*m_pRefCount);
		}
	}
		
	void	Set( const char* str )
	{
		if ( str )
		{
			m_length = strlen( str );
			Allocate();
			strcpy( m_str, str );
			ComputeHash();
		}
	}
	
	void	SetTruncate( const char* str, size_t length )
	{
		if ( str )
		{
			m_length = length;
			Allocate();
			strncpy( m_str, str, length );
			m_str[ length ] = '\0';
			ComputeHash();			
		}
	}
		
	void	Allocate()
	{
		m_str = new char[ m_length + 1 ];
		m_pRefCount = new int(1);
	}
		
	void	ComputeHash()
	{
		if ( m_str == NULL )
		{
			m_hash = 0;
			return;
		}

		size_t _Val = 2166136261U;
		size_t _First = 0;
		size_t _Last = m_length;
		size_t _Stride = 1 + _Last / 10;

		for(; _First < _Last; _First += _Stride)
			_Val = 16777619U * _Val ^ (size_t)m_str[_First];
			
		m_hash = _Val;
	}
	
	char*	m_str;
	size_t	m_length;
	int*	m_pRefCount;
	size_t	m_hash;
};

typedef CString String;

WHITEBOX_END

#ifdef __APPLE__

#include <ext/hash_map>

namespace __gnu_cxx
{
	using namespace std;

	template<>
	struct hash< WhiteBox::String >
	{
		size_t operator()( const WhiteBox::String& str ) const
		{
			return str.getHash();
		}
	};
}

#elif defined( WIN32 )

#include <xfunctional>
#include <xhash>

namespace std
{
	template<>
	class hash< WhiteBox::CString >
	 : public unary_function< WhiteBox::CString, size_t >
	{	// hash functor
	public:
		 typedef WhiteBox::CString _Kty;

		size_t operator()(const _Kty& _Keyval) const
		{
			return _Keyval.getHash();
		}
	};
}

namespace stdext
{
	template<>
	class hash_compare< WhiteBox::CString >
	{
	public :
		static const size_t bucket_size = 4;
		static const size_t min_buckets = 8;

		size_t operator()(const WhiteBox::CString &mc) const
		{
			return mc.getHash();
		}

		bool operator()(const WhiteBox::CString &mc1, const WhiteBox::CString &mc2) const
		{
			return (mc1.getHash() < mc2.getHash());
		}
	};
}

#endif

#endif


 