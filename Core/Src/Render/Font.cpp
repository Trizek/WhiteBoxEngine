#include <ft2build.h>
#include FT_FREETYPE_H

#include "Render/Font.h"

#include "FontSystem.h"
#include "GlobalVariables.h"
#include "Render/Renderer.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

CFont::CFont(  CDataStream& dataStream, uint size /*= 16*/, uint dpi /*= 300*/ )
{
	uint charCount = 647;
	m_characters.resize( charCount );

	if ( !gVars->pFontSystem->IsInit() )
	{
		WbLog( "Font", "Font system not init !!" );
		return;
	}

	FT_Face face;
	int error = FT_New_Memory_Face( gVars->pFontSystem->GetLibrary(), (FT_Byte*)dataStream.GetCursorData(), dataStream.GetSize(), 0, &face );
	if ( error != 0 )
	{
		return;
	}

	error = FT_Set_Char_Size( face, 0, size * 64, dpi, dpi );
	if ( error != 0 )
	{
		return;
	}

	uint textureWidth = 2048;
	uint textureHeight = 2048;

	uint offsetX = 0, offsetY = 0;

	uint jumpY = 0;

	unsigned char* pixels = new unsigned char[ textureWidth * textureHeight * 4 ];
	memset(pixels, 0xFF, textureWidth * textureHeight * 4);

	for ( uint character = 0; character < charCount; ++character )
	{
		int glyph_index = FT_Get_Char_Index( face, character );

		error = FT_Load_Glyph( face, glyph_index, 0);
		error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

		FT_GlyphSlot slot = face->glyph;

		uint charHeight = slot->bitmap.rows;
		uint charWidth = slot->bitmap.width;

		jumpY = Max(jumpY, charHeight);

		unsigned char pixelmode = slot->bitmap.pixel_mode;

		if ( offsetX + charWidth >= textureWidth )
		{
			offsetX = 0;
			offsetY += jumpY + 20;
			jumpY = 0;
		}

		if ( offsetY + charHeight >= textureHeight )
		{
			WbLog( "Default", "Texture too small for font !!\n");
			break;
		}

		for ( uint j = 0; j < charHeight; ++j )
		{
			for ( uint i = 0; i < charWidth; ++i )
			{
				unsigned char val = slot->bitmap.buffer[j * charWidth + i];

				float color = (float(val)) / (float)0xFF;

				uint index = (offsetY + j) * textureWidth + (offsetX + i);
				pixels[4 * index] = (unsigned char)(0xFF * (1.0f - color));
				pixels[4 * index + 1] = (unsigned char)(0xFF * (1.0f - color));
				pixels[4 * index + 2] = (unsigned char)(0xFF * (1.0f - color));
				pixels[4 * index + 3] = (unsigned char)(0xFF * (color));

			}
		}

		m_characters[ character ] = SCharacter( Vec2( (float)offsetX, (float)offsetY ), Vec2( (float)slot->bitmap_left, (float)slot->bitmap_top ), Vec2( float(charWidth), float(charHeight) ), (float)(slot->advance.x >> 6) );

		offsetX += charWidth + 20;
	}

	m_uvScale.x = 1.0f / (float)textureWidth;
	m_uvScale.y = 1.0f / (float)textureHeight;

	m_pMaterial = CMaterialPtr( new CMaterial() );
	m_pMaterial->m_textureLayers[0].m_pTexture = CTexturePtr( new CTexture(textureWidth, textureHeight, 0, CPicture::ePF_R8G8B8A8, pixels) );

	delete[] pixels;
}

const CFont::SCharacter&			CFont::GetCharater( wchar unicode ) const
{
	return m_characters[ (uint)unicode ];
}

const Vec2&					CFont::GetUVScale() const
{
	return m_uvScale;
}

CMaterialPtr					CFont::GetMaterial() const
{
	return m_pMaterial;
}

WHITEBOX_END
