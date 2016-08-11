#ifndef __RENDERTARGET_H__
#define __RENDERTARGET_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class IRenderTarget : public CSmartPointerData
{
public:
	IRenderTarget( uint width, uint height )
		: m_width(width)
		, m_height(height){}

	uint	GetWidth() const {	return m_width; }
	uint	GetHeight() const { return m_height; }
	
	void	SetWidth( uint width ){	m_width = width; }
	void	SetHeight( uint height ){ m_height = height; }	
	
	virtual void	Bind() = 0;
	
private:
	uint	m_width;
	uint	m_height;
};

DEFINE_SMART_PTR( IRenderTarget );

WHITEBOX_END

#endif