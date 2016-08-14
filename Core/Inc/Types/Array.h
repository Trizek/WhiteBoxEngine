#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "WhiteBoxNameSpace.h"

WHITEBOX_BEGIN


template<class T, bool bConstructObjects = true>
class Array
{
public:
	Array() : m_Elements(nullptr), m_Size(0) {}
	~Array() { Deallocate(); }

	size_t	GetSize() const { return m_Size; }

	void	SetSize(size_t size)
	{
		if (size != m_Size)
		{
			Allocate(size);
		}
	}

	const T& operator[](size_t index) const { return m_Elements[index]; }
	T& operator[](size_t index) { return m_Elements[index]; }

private:
	void	Allocate(size_t capacity)
	{
		T*	elements;
		if (bConstructObjects)
		{
			elements = new T[capacity]; // call constructors
		}
		else
		{
			elements = reinterpret_cast<T*>(new char[sizeof(T) * capacity]); // don't call constructors
		}
		
		m_Elements = elements;
		m_Size = capacity;
	}

	void	Deallocate()
	{
		if (m_Elements != nullptr)
		{
			if (bConstructObjects)
			{
				delete[] m_Elements; // call destructors
			}
			else
			{
				delete[] reinterpret_cast<char*>(m_Elements); // don't call destructors
			}
		}
	}

	T*		m_Elements;
	size_t	m_Size;
};

WHITEBOX_END

#endif