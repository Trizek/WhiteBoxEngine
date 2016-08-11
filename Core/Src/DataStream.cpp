#include "DataStream.h"

#include "FileSystem.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

CDataStream::CDataStream( void* pData, size_t size )
	: m_pData(pData)
	, m_size(size)
	, m_cursor(0){}

size_t	CDataStream::GetCursor() const
{
	return m_cursor;
}

bool	CDataStream::SetCursor( size_t cursor )
{
	m_cursor = cursor;
	return true;
}

size_t	CDataStream::GetSize() const
{
	return m_size;
}

void*	CDataStream::GetCursorData()
{
	return (char*)m_pData + m_cursor;
}

bool	CDataStream::Read( size_t elementSize, size_t elemCount, void* pDest )
{
	bool bSuccess = true;

	size_t size = elementSize * elemCount;
	if ( size > m_size - m_cursor )
	{
		size = m_size - m_cursor;
		bSuccess = false;
	}
	
	memcpy( pDest, (char*)m_pData + m_cursor, size );
	m_cursor += size;
	
	return bSuccess;
}

bool	CDataStream::Write( size_t elementSize, size_t elemCount, const void* pSource )
{
	bool bSuccess = true;

	size_t size = elementSize * elemCount;
	if ( size > m_size - m_cursor )
	{
		size = m_size - m_cursor;
		bSuccess = false;
	}
	
	memcpy( (char*)m_pData + m_cursor, pSource, size );
	m_cursor += size;
	
	return true;
}

bool	CDataStream::ReadByte( char& byte )
{
	if ( m_cursor == m_size )
	{
		return false;
	}
	
	byte = ((char*)m_pData)[ m_cursor++ ];
	return true;
}

bool	CDataStream::WriteByte( char byte )
{
	if ( m_cursor == m_size )
	{
		return false;
	}
	
	((char*)m_pData)[ m_cursor++ ] = byte;
	return true;
}

bool	CDataStream::ReadString( String& str )
{
	size_t length = 0;
	bool bSuccess = Read( sizeof(size_t), 1, &length );
	if ( length > 0 )
	{
		char* strBuf = new char[ length + 1 ];
		strBuf[ length ] = '\0';
		bSuccess = bSuccess && Read( sizeof(char), length, strBuf );
		str = strBuf;
		delete[] strBuf;
	}
	
	return bSuccess;
}

void	CDataStream::Destroy()
{
	delete[] (char*)m_pData;
	m_pData = NULL;
	m_size = 0;
	m_cursor = 0;
}

CDataStream	CDataStream::GetStreamFromFile( const String& path, size_t size )
{
	TFileHandle file = gVars->pFileSystem->OpenFile( path.c_str(), true, false );
	if ( size == 0 )
	{
		size = gVars->pFileSystem->GetFileSize( file );
		gVars->pFileSystem->SetCursor( file, 0 );
	}
	char* pData = new char[ size + 1 ];
	pData[ size ] = '\0'; 
	gVars->pFileSystem->Read( file, 1, size, pData );
	gVars->pFileSystem->CloseFile( file );
	
	return CDataStream( pData, size );
}
	
WHITEBOX_END
