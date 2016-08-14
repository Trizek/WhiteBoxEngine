#ifndef __SPARSEARRAY_H__
#define __SPARSEARRAY_H__

#include "WhiteBoxNameSpace.h"
#include "BitArray.h"

WHITEBOX_BEGIN

template<class T>
class SparseArray
{
public:
	SparseArray() : m_FrontFreeBockIndex(-1), m_BackFreeBlockIndex(0) {}

	~SparseArray()
	{
		for (Iterator it = GetIterator(); it.IsValid(); it.MoveNext())
		{
			(*it).~T();
		}
	}

	size_t	GetSize() const { return m_Elements.GetSize(); }

	void	SetSize(size_t size) { m_Elements.SetSize(size); m_ValidIndices.SetSize(size); }

	const T& operator[](size_t index) const { return m_Elements[index]; }
	T& operator[](size_t index) { return m_Elements[index]; }

	template<class Tval>
	int		AddElement(Tval&& element)
	{
		size_t index = -1;
		if (AllocateBlock(index))
		{
			new (&m_Elements[index]) T(std::forward<Tval>(element));
			m_ValidIndices.SetBit(index, true);
		}

		return index;
	}

	void	RemoveElement(int index)
	{
		if (m_ValidIndices.GetBit((size_t)index))
		{
			m_ValidIndices.SetBit((size_t)index, false);
			m_Elements[index].~T();
			FreeBlock(index);
		}
	}

public:

	template< bool bConst >
	class IteratorGeneric
	{
	public:
		typedef SparseArray<T>								SparseArrayType;
		typedef typename Reference< SparseArrayType, bConst >::Ref	SparseArrayRef;
		typedef typename Reference< T, bConst >::Ref				ElementRef;

		IteratorGeneric( SparseArrayRef sparseArray ) : m_Index(-1), m_Array(sparseArray)
		{
			MoveNext();
		}

		bool	IsValid() const
		{
			return m_Index >= 0;
		}

		void	MoveNext()
		{
			m_Index = m_Array.m_ValidIndices.FindFirstSetBit((size_t)(++m_Index));
		}

		ElementRef		operator*()
		{
			return m_Array[ m_Index ];
		}

	private:
		int				m_Index;
		SparseArrayRef	m_Array;
	};

	typedef IteratorGeneric< true > ConstIterator;
	typedef IteratorGeneric< false > Iterator;


	template< bool bConst >
	IteratorGeneric< bConst >	GetIteratorGeneric()
	{
		return IteratorGeneric< bConst >( *this );
	}

	Iterator	GetIterator()
	{
		return Iterator(*this);
	}

	ConstIterator	GetConstIterator() const
	{
		return ConstIterator(*this);
	}


private:
	bool		AllocateBlock(size_t& index)
	{
		if (m_FrontFreeBockIndex >= 0)
		{
			index = (size_t)m_FrontFreeBockIndex;
			int* nextFreeBlockIndexPtr = reinterpret_cast<int*>(&m_Elements[m_FrontFreeBockIndex]);
			m_FrontFreeBockIndex = *nextFreeBlockIndexPtr;

			return true;
		}

		if (m_BackFreeBlockIndex < (int)m_Elements.GetSize())
		{
			index = (size_t)m_BackFreeBlockIndex++;

			return true;
		}

		return false;
	}

	void		FreeBlock(size_t index)
	{
		if (index >= (size_t)m_BackFreeBlockIndex)
		{
			return;
		}


		int* nextFreeBlockIndexPtr = reinterpret_cast<int*>(&m_Elements[index]);
		*nextFreeBlockIndexPtr = m_FrontFreeBockIndex;
		m_FrontFreeBockIndex = (int)index;
	}


	Array<T, false>		m_Elements;
	BitArray			m_ValidIndices;

	int					m_FrontFreeBockIndex;
	int					m_BackFreeBlockIndex;
};

WHITEBOX_END

#endif