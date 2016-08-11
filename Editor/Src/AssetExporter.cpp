#include "AssetExporter.h"

WHITEBOX_BEGIN

bool IAssetExporter::IsDigit( char c )
{
	return ( c >= '0' && c <= '9' );
}

float IAssetExporter::ToFloat( const char* str )
{
	return (float)atof( str );
}

long int IAssetExporter::ToInt( const char* str )
{
	return atoi( str );
}

bool IAssetExporter::ReadWord( TFileHandle file, char* pDest )
{
	bool bFound = false;
	while( true )
	{
		if ( !gVars->pFileSystem->ReadByte( file, *pDest ) )
		{
			*pDest = '\0';
			return bFound;
		}

		if ( *pDest == ' ' ||  *pDest == '\t' || *pDest == '\r' || *pDest == '\n' || *pDest == '\r' )
		{
			if ( bFound )
			{
				*pDest = '\0';
				return bFound;
			}
		}
		else
		{
			bFound = true;
			++pDest;
		}
	}
	
	return bFound;
}

void IAssetExporter::SkipLine( TFileHandle file )
{
	char c;
	while ( gVars->pFileSystem->ReadByte( file, c ) && (c != '\n') );
}

WHITEBOX_END
