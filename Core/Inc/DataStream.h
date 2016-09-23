#ifndef __DATASTREAM_H__
#define __DATASTREAM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CDataStream
{
public:
	CDataStream();
	CDataStream( void* pData, size_t size );
	
	size_t	GetCursor() const;
	bool	SetCursor( size_t cursor );
	size_t	GetSize() const;
	
	void*	GetCursorData();
	
	bool	Read( size_t elementSize, size_t elemCount, void* pDest );
	bool	Write( size_t elementSize, size_t elemCount, const void* pSource );
	
	bool	ReadByte( char&	byte );
	bool	WriteByte( char byte );

	char	operator++();
	char	operator+=( size_t offset );
	char	Get() const;
	bool	EndOfStream() const;
	
	void	Destroy();
	
	template< class Type >
	bool	Read( Type& object )
	{
		if ( m_size - m_cursor < sizeof(object) )
		{
			return false;
		}
		object = *((Type*)( (char*)m_pData + m_cursor ));
		m_cursor += sizeof(Type);
		
		return true;
	}
	
	template< class Type >
	bool	Write( Type& object )
	{
		if ( m_size - m_cursor < sizeof(object) )
		{
			return false;
		}
		*((Type*)( (char*)m_pData + m_cursor )) = object;
		m_cursor += sizeof(Type);
		
		return true;
	}
	
	bool	ReadString( String& str );
	
private:
	void*	m_pData;
	size_t	m_size;
	
	size_t	m_cursor;
};

WHITEBOX_END

#endif