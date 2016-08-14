#ifndef __BITARRAY_H__
#define __BITARRAY_H__



#include "Array.h"
#include <memory>
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)

WHITEBOX_BEGIN

class BitArray
{
public:

	typedef unsigned int	Word;
	enum
	{
		WordSize = sizeof(Word) * 8,
		WordMask = WordSize - 1,
	};

	void	SetSize(size_t size)
	{
		size = (1 << (size_t)ceil(log2(size)));

		size_t wordArraySize = (size_t)ceilf((float)size / (float)BitArray::WordSize);
		m_WordArray.SetSize(wordArraySize);
		memset(&m_WordArray[0], 0, sizeof(Word) * wordArraySize);
	}

	bool	GetBit(size_t index) const
	{
		size_t wordIndex = index / WordSize;
		size_t bitPosition = index & WordMask;

		return ((m_WordArray[wordIndex] & (1 << bitPosition)) > 0);
	}

	void	SetBit(size_t index, bool set)
	{
		size_t wordIndex = index / WordSize;
		size_t bitPosition = index & WordMask;

		Word word = m_WordArray[wordIndex];
		Word mask = 1 << bitPosition;
		word &= ~mask;
		word |= mask * (int)set;

		m_WordArray[wordIndex] = word;
	}

	int		FindFirstSetBit(size_t startIndex) const
	{
		size_t wordIndex = startIndex / WordSize;
		size_t bitPosition = startIndex & WordMask;

		while (wordIndex < m_WordArray.GetSize() && m_WordArray[wordIndex] == 0)
		{
			++wordIndex;
			bitPosition = 0;
		}

		if (wordIndex == m_WordArray.GetSize())
		{
			return -1;
		}

		unsigned long bitIndex = 0;
		return (_BitScanForward(&bitIndex, m_WordArray[wordIndex] >> bitPosition)) ? (int)(wordIndex * WordSize + (size_t)bitIndex + bitPosition) : -1;
	}

	void Print()
	{
		printf("[");
		for (size_t i = 0; i < m_WordArray.GetSize(); ++i)
		{
			for (size_t j = 0; j < WordSize; ++j)
			{
				int bit = (int)((m_WordArray[i] & (1 << j)) > 0);
				printf("%d", bit);
			}
		}
		printf("]\n");
	}

private:
	Array<Word>	m_WordArray;

};

WHITEBOX_END

#endif