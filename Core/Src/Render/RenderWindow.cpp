#include "Render/RenderWindow.h"

#include "Render/Renderer.h"
#include "System.h"

WHITEBOX_BEGIN

CRenderWindow::CRenderWindow( uint x, uint y, uint width, uint height )
	: IRenderTarget( width, height )
	, m_x(x)
	, m_y(y){}

uint	CRenderWindow::GetX() const
{
	return m_x;
}

uint	CRenderWindow::GetY() const
{
	return m_y;
}

void	CRenderWindow::SetX( uint x )
{
	m_x = x;
}

void	CRenderWindow::SetY( uint y )
{
	m_y = y;
}

void	CRenderWindow::Bind()
{
	gSystem->pRenderer->BindRenderWindow( this );
}

WHITEBOX_END