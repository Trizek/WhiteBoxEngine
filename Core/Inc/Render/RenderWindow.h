#ifndef __RENDERWINDOW_H__
#define __RENDERWINDOW_H__

#include "RenderTarget.h"

WHITEBOX_BEGIN

class CRenderWindow : public IRenderTarget
{
public:
	CRenderWindow( uint x, uint y, uint width, uint height );
	
	uint	GetX() const;
	uint	GetY() const;
	
	void	SetX( uint x );
	void	SetY( uint y );
	
	virtual void	Bind();
	
private:
	uint	m_x;
	uint	m_y;
};

WHITEBOX_END

#endif