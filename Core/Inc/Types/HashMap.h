#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "SparseArray.h"

WHITEBOX_BEGIN

template<class T>
class PrintType
{
public:
	static void Print(const T& val) { WbLog( "Default", "undef"); };
};

template<>
class PrintType<int>
{
public:
	static void Print(const int& val) { WbLog( "Default", "%d", val); };
};


template<class T>
class DefaultHashFunction
{
public:
	static int ComputeHash(const T& value)
	{
		return 0;
	}
};


template<>
class DefaultHashFunction<int>
{
public:
	static int ComputeHash(const int& value)
	{
		return value;
	}
};


template<class Key, class Value, class HashFunction = DefaultHashFunction<Key>>
class HashMap
{
public:
	typedef HashMap< Key, Value > HashMapType;
	typedef Value*	FindRes;

	HashMap(size_t size = 512)
	{
		//size = (1 << (size_t)ceil(log2(size)));

		m_Pairs.SetSize(size);
		m_HashIndices.SetSize(size);
		memset(&m_HashIndices[0], -1, sizeof(int) * size);

		m_SizeMask = size - 1;
	}

	struct MapElement
	{
		MapElement() : nextElementId(-1) {}
		MapElement(const Key& _key, const Value& _value) : key(_key), value(_value), nextElementId(-1) {}

		MapElement(MapElement&& rhs) : key(std::move(rhs.key)), value(std::move(rhs.value)), nextElementId(-1) {}

		MapElement& operator=(MapElement&& rhs)
		{
			key = std::move(rhs.key);
			value = std::move(rhs.value);
			return *this;
		}

		Key		key;
		Value	value;

		int		nextElementId; // next elem in the bucket
	};

	int		ComputeHash(const Key& key) const
	{
		return	HashFunction::ComputeHash(key) & m_SizeMask;
	}

	Value&	operator[](const Key& key)
	{
		MapElement* element = FindMapElement(key);
		if ( element == nullptr )
		{
			element = AddPair( key, Value() );
		}
		return element->value;
	}

	const Value& operator[]( const Key& key ) const
	{
		return *FindElement( key );
	}

	Value*	FindElement( const Key& key )
	{
		MapElement* element = FindMapElement( key );
		return ( element == nullptr )? nullptr : &(element->value);
	}

	const Value* FindElement( const Key& key ) const
	{
		return const_cast< HashMapType& >( *this ).FindElement( key );
	}

	MapElement*	AddPair( const Key& key, const Value& value )
	{
		int hash = ComputeHash(key);
		int index = m_HashIndices[hash];
		if (index < 0)
		{
			// Hash no present, create new bucket with only new element inside
			int hashIndex = m_Pairs.AddElement(MapElement(key, value));
			m_HashIndices[hash] = hashIndex;
			return &m_Pairs[ hashIndex ];
		}

		while( true )
		{
			MapElement& pair = m_Pairs[index];
			if (pair.key == key)
			{
				// Element already present, update value
				pair.value = value;
				return &pair;
			}
			if (pair.nextElementId < 0.0f)
			{
				// reached last element of the bucket, adding new element
				pair.nextElementId = m_Pairs.AddElement(MapElement(key, value));
				return &m_Pairs[ pair.nextElementId ];
			}

			index = pair.nextElementId;
		}

		return nullptr;
	}

	void	RemoveElement(const Key& key)
	{
		int hash = ComputeHash(key);
		int index = m_HashIndices[hash];

		if (index < 0)
		{
			// element not present in the map
			return;
		}

		MapElement* pairPtr = &m_Pairs[index];
		if (pairPtr->key == key)
		{
			// first element of the bucket, map bucket's head to next element
			m_HashIndices[hash] = pairPtr->nextElementId;
			m_Pairs.RemoveElement(index);
		}
		else
		{
			while ((index = pairPtr->nextElementId) >= 0)
			{
				MapElement* nextPairPtr = &m_Pairs[index];
				if (nextPairPtr->key == key)
				{
					pairPtr->nextElementId = nextPairPtr->nextElementId;
					m_Pairs.RemoveElement(index);
					break;
				}
				pairPtr = &m_Pairs[index];
			}
		}
	}

	void PrintBuckets()
	{
		for (size_t iBucket = 0; iBucket < m_HashIndices.GetSize(); ++iBucket)
		{
			int index = m_HashIndices[iBucket];
			if (index < 0)
			{
				continue;
			}

			WbLog( "Default", "Hash %d: ", iBucket);
			for (; index >= 0; index = m_Pairs[index].nextElementId)
			{
				WbLog( "Default", "[");
				PrintType<Key>::Print(m_Pairs[index].key);
				WbLog( "Default", ",");
				PrintType<Value>::Print(m_Pairs[index].value);
				WbLog( "Default", "]");
			}
			WbLog( "Default", "\n");
		}
	}

	float	GetAverageBucketSize()
	{
		size_t total = 0;
		size_t count = 0;
		for (size_t iBucket = 0; iBucket < m_HashIndices.GetSize(); ++iBucket)
		{
			int index = m_HashIndices[iBucket];
			if (index < 0)
			{
				continue;
			}

			++count;
			for (; index >= 0; index = m_Pairs[index].nextElementId)
			{
				++total;
			}
		}

		if (count > 0)
		{
			return ((float)total) / ((float)count);
		}
		else
		{
			return 0.0f;
		}
	}

	template< bool bConst, class SparseArrayIterator >
	class IteratorGeneric
	{
	public:
		typedef typename Reference< Value, bConst >::Ref						ValueRef;

 		IteratorGeneric( SparseArrayIterator iterator ) : m_Iterator( iterator )
		{}
 
 		bool	IsValid() const
 		{
			return m_Iterator.IsValid();
 		}
 
 		void	MoveNext()
 		{
 			m_Iterator.MoveNext();
 		}
 
 		const Key& GetKey()
 		{
 			return (*m_Iterator).key;
 		}
 
 		ValueRef GetValue()
 		{
 			return (*m_Iterator).value;
 		}
 
 	private:
 		SparseArrayIterator	m_Iterator;
	};


	typedef IteratorGeneric< true, typename SparseArray<MapElement>::ConstIterator > ConstIterator;
	typedef IteratorGeneric< false, typename SparseArray<MapElement>::Iterator > Iterator;


	Iterator	GetIterator()
	{
		return Iterator(m_Pairs.GetIterator());
	}

	ConstIterator	GetConstIterator() const
	{
		return ConstIterator(m_Pairs.GetConstIterator());
	}


private:
	MapElement*	FindMapElement( const Key& key )
	{
		int hash = ComputeHash( key );
		int index = m_HashIndices[ hash ];

		while (index >= 0)
		{
			MapElement& pair = m_Pairs[ index ];
			if (pair.key == key)
			{
				return &pair;
			}
			else
			{
				index = pair.nextElementId;
			}
		}

		return nullptr;
	}


	SparseArray< MapElement >	m_Pairs;
	Array< int >				m_HashIndices;
	size_t						m_SizeMask;
};

WHITEBOX_END

#define Map HashMap

#endif
