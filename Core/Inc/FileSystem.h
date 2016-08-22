#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

typedef void* TFileHandle;

class IFileBrowser
{
public:
	virtual void OnBrowseFile( const String& filePath ) = 0;
};

class CFileSystem
{
public:
	TFileHandle	OpenFile( const char* path, bool bRead, bool bWrite );
	void	CloseFile( TFileHandle fileHandle );
	
	size_t	GetCursor( TFileHandle fileHandle );
	bool	SetCursor( TFileHandle fileHandle, size_t cursor );
	size_t	GetFileSize( TFileHandle fileHandle );
	
	bool	Read( TFileHandle fileHandle, size_t elementSize, size_t elemCount, void* pDest );
	bool	Write( TFileHandle fileHandle, size_t elementSize, size_t elemCount, const void* pSource );
	
	bool	ReadByte( TFileHandle fileHandle, char&	byte );
	bool	WriteByte( TFileHandle fileHandle, char byte );

	bool	WriteString( TFileHandle fileHandle, const String& str )
	{
		size_t length = str.length();
		bool bSuccess = Write( fileHandle, sizeof(size_t), 1, &length );
		if ( length > 0 )
		{
			bSuccess = bSuccess && Write( fileHandle, sizeof(char), length, str.c_str() );
		}
		
		return bSuccess;
	}
	
	// File manip. optional
	void	BrowseDirectory( const String& dirPath, IFileBrowser& fileBrowser );
	void	RemoveFile( const String& filePath );
	void	MoveFileTo( const String& oldPath, const String& newPath );
	void	CopyFileTo( const String& sourcePath, const String& destinationPath );
	bool	DoesFileExist(const String& filePath);
	bool	DoesDirExist( const String& dirPath );
	void	MakeDir( const String& dirPath );
	void	CreateFileDir( const String& filePath );
};

WHITEBOX_END

#endif