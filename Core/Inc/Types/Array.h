#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "WhiteBoxNameSpace.h"

WHITEBOX_BEGIN


template< class T, bool bConstructObject = true >
class Array
{
public:
	Array() : m_Elements(nullptr), m_Size(0), m_Capacity(0) {}
	~Array() { Deallocate(); }

	size_t	GetSize() const { return m_Size; }

	void	SetSize(size_t size)
	{
		if (size > m_Capacity)
		{
			Allocate( m_Capacity == 0 ? size * 2 : m_Capacity * 2 );
		}
		
		m_Size = size;
	}

	const T& operator[](size_t index) const { return m_Elements[index]; }
	T& operator[](size_t index) { return m_Elements[index]; }

private:
	void	Allocate(size_t capacity)
	{
		T*	elements;
		elements = reinterpret_cast<T*>(new char[sizeof(T) * capacity]); // don't call constructors

		if ( m_Elements != nullptr )
		{
			if ( bConstructObject )
			{
				for( size_t i = 0; i < m_Size; ++i )
				{
					new (&elements[i]) T(std::move(m_Elements[i]));
				}
			}
			
			Deallocate();
		}
		
		m_Elements = elements;
		m_Capacity = capacity;
	}

	void	Deallocate()
	{
		if (m_Elements != nullptr)
		{
			if ( bConstructObject )
			{
				for ( size_t i = 0; i < m_Size; ++i )
				{
					m_Elements[i].~T();
				}
			}
			delete[] reinterpret_cast<char*>(m_Elements); // don't call destructors
		}
	}

	T*		m_Elements;
	size_t	m_Size, m_Capacity;
};

WHITEBOX_END

#endif