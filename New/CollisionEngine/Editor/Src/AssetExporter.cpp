#include "AssetExporter.h"

WHITEBOX_BEGIN

bool IAssetExporter::IsDigit( char c )
{
	return ( c >= '0' && c <= '9' );
}

float IAssetExporter::ToFloat( const char* str )
{
	float val = 0.0f;
	float neg = 1.0f;
	if ( *str == '-' )
	{
		neg = -1.0f;
		++str;
	}

	while( IsDigit( *str ) )
	{
		val = 10.0f * val + (float)(*str - '0');
		++str;
	}
	
	if ( *str == '\0' )
	{
		return val * neg;
	}
	
	if ( *str == '.' )
	{
		++str;
		float div = 10.0f;
		while( IsDigit( *str ) )
		{		
			val += ( (float)(*str - '0') ) / div;
			div *= 10.0f;
			++str;
		}
	}
	bool bPrint = false;
	if ( *str == 'e' || *str == 'E' )
	{
		++str;
		float step = 10.0f;
		if ( *str == '-' )
		{
			step = 1.0f/10.0f;
			++str;
		}
		
		int pow = 0;
		while( IsDigit( *str ) )
		{		
			pow = 10 * pow + ( (int)(*str - '0') );
			++str;
		}
	
		for( int i=0 ; i < pow ; ++i )
		{
			val *= step;
		}
	}
	
	return val * neg;
}

long int IAssetExporter::ToInt( const char* str )
{
	long coeff = 1;
	if ( *str == '-' )
	{
		coeff = -1;
		++str;
	}

	long int val = 0;
	while( IsDigit( *str ) )
	{
		val = 10 * val + (int)(*str - '0');
		++str;
	}
	
	return val * coeff;
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
