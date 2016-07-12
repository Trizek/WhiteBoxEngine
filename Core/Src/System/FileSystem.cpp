#include "FileSystem.h"

#include <stdio.h>

WHITEBOX_BEGIN


TFileHandle	CFileSystem::OpenFile( const char* path, bool bRead, bool bWrite )
{
	const char* mode;
	if ( bRead && bWrite )
	{
		mode = "wb+";
	}
	else if ( bWrite )
	{
		mode = "wb";
	}
	else
	{
		mode = "rb";
	}

	FILE* pFile = nullptr;
	fopen_s( &pFile, path, mode );
	return static_cast< TFileHandle >( pFile );
}

void	CFileSystem::CloseFile( TFileHandle fileHandle )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		fclose( pFile );
	}
}

size_t	CFileSystem::GetCursor( TFileHandle fileHandle )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		return (size_t)ftell( pFile );
	}
	
	return 0;
}

bool	CFileSystem::SetCursor( TFileHandle fileHandle, size_t cursor )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		return ( fseek( pFile, (long)cursor, SEEK_SET ) == 0 );
	}	
	
	return false;
}

size_t	CFileSystem::GetFileSize( TFileHandle fileHandle )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		size_t size = 0;
		
		// cache cursor
		size_t cursor = (size_t)ftell( pFile );
		
		// go to end file
		fseek( pFile, 0, SEEK_END );
		
		// size = cursor
		size = (size_t)ftell( pFile );
		
		// restore cursor
		fseek( pFile, (long)cursor, SEEK_SET );
		
		return size;
	}
	
	return 0;
}

bool	CFileSystem::Read( TFileHandle fileHandle, size_t elementSize, size_t elemCount, void* pDest )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		return ( (size_t)fread( pDest, elementSize, elemCount, pFile ) == elemCount );
	}	
	
	return false;
}

bool	CFileSystem::Write( TFileHandle fileHandle, size_t elementSize, size_t elemCount, const void* pSource )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		return ( (size_t)fwrite( pSource, elementSize, elemCount, pFile ) == elemCount );
	}	
	
	return false;
}

bool	CFileSystem::ReadByte( TFileHandle fileHandle, char&	byte )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		int read = fgetc( pFile );
		if ( read == EOF )
		{
			return false;
		}
		
		byte = (char)read;
		return true;
	}	
	
	return false;	
}

bool	CFileSystem::WriteByte( TFileHandle fileHandle, char byte )
{
	FILE* pFile = static_cast< FILE* >( fileHandle );
	if ( pFile != NULL )
	{
		return ( fputc( byte, pFile ) != EOF );
	}	
	
	return false;	
}


#if 0

#include <unistd.h>


#include <errno.h> // for errno
#include <stdio.h> // for printf
#include <stdlib.h> // for EXIT_*
#include <string.h> // for strerror
#include <sys/stat.h> // for stat and mkdir
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <dirent.h>


int file_select( struct dirent* entry )
{
	if (	( strcmp(entry->d_name, ".") == 0) ||
			( strcmp(entry->d_name, "..") == 0) )
	{
		return 0;
	}
	
	return 1;
}

bool IsADir( const char* path )
{
	DIR* dirp = opendir( path );
	bool bDir = dirp != NULL;
	if ( bDir )
	{
		(void)closedir( dirp );
	}

	return bDir;
}

void	CFileSystem::BrowseDirectory( const String& dirPath, IFileBrowser& fileBrowser )
{
	struct direct **files;
	int fileCount = scandir( dirPath.c_str(), &files, file_select, alphasort );
	for ( int iFile = 1 ; iFile < fileCount + 1 ; ++iFile )
	{		
		String filePath;
		if ( dirPath.length() > 0 && dirPath.c_str()[ dirPath.length() - 1 ] != '/' )
		{
			filePath = dirPath + "/" + String(files[ iFile - 1 ]->d_name);
		}
		else
		{
			filePath = dirPath + String(files[ iFile - 1 ]->d_name);
		}
		
		if ( IsADir( filePath.c_str() ) )
		{
			BrowseDirectory( filePath, fileBrowser );
		}
		else
		{
			fileBrowser.OnBrowseFile( filePath );
		}
	}		 
}

void	CFileSystem::RemoveFile( const String& filePath )
{
	remove( filePath.c_str() );
}

void	CFileSystem::MoveFile( const String& oldPath, const String& newPath )
{
	rename( oldPath.c_str(), newPath.c_str() );
}

void	CFileSystem::CopyFile( const String& sourcePath, const String& destinationPath )
{
	// Read
	TFileHandle fileHandle = OpenFile( sourcePath.c_str(), true, false );
	if ( fileHandle == NULL )
	{
		return;
	}
	size_t size = GetFileSize( fileHandle );
	char* fileData = new char[ size ];
	Read( fileHandle, 1, size, fileData );
	CloseFile( fileHandle );
	
	// Write
	fileHandle = OpenFile( destinationPath.c_str(), false, true );
	Write( fileHandle, 1, size, fileData );
	CloseFile( fileHandle );
	
	delete[] fileData;
}

bool	CFileSystem::DoesDirExist( const String& dirPath )
{
	struct stat st;
    return ( stat( dirPath.c_str(), &st ) == 0 );
}

void	CFileSystem::MakeDir( const String& dirPath )
{
	mkdir( dirPath.c_str(), S_IRWXU|S_IRWXG ); 
}

void	CFileSystem::CreateFileDir( const String& filePath )
{
	String dirPath = filePath.get_path_base();
	String dirToCreate;
	while( !dirPath.empty() )
	{
		size_t slashPos = dirPath.find( '/' );
		if ( slashPos == String::npos || slashPos == 0 )
		{
			return;
		}
		
		String dirName = dirPath.substr( 0, slashPos );
		dirToCreate = dirToCreate + dirName + "/";
		dirPath = dirPath.substr( slashPos + 1 );
		
		if ( !DoesDirExist( dirToCreate ) )
		{
			MakeDir( dirToCreate );
		}
	}
}

#endif


WHITEBOX_END
