#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "BaseTypes.h"
#include "SmartPointer.h"

#define CLAMP_HANDLE_INDEX(index1)

WHITEBOX_BEGIN

typedef unsigned int	Handle;
typedef unsigned short	HandleIndexType;

template< class Type, size_t Size >
class CHandleManager
{
public:
	CHandleManager()
	{
		Reset();
	}

	static HandleIndexType	HandleToArrayIndex( Handle handle )
	{
		return HandleIndexType(handle & 0xFFFF);
	}

	static HandleIndexType	HandleToDescendant( Handle handle )
	{
		return HandleIndexType(handle >> 16);
	}

	static Handle	BuildHandle( HandleIndexType index, HandleIndexType descendant )
	{
		return (Handle(descendant) << 16) | Handle(index);
	}


	CSmartPointer< Type > GetData( const Handle handle )
	{
		HandleIndexType index = HandleToArrayIndex( handle );
		CLAMP_HANDLE_INDEX(index);

		SHandleSlot& slot = m_handleSlots[ index ];
		if ( HandleToDescendant( handle ) == slot.m_descendant )
		{
			return m_handleSlots[ index ].m_pData;
		}
		else
		{
			return NULL;
		}
	}

	bool	SetData( Handle handle, Type& data )
	{
		HandleIndexType index = HandleToArrayIndex( handle );
		CLAMP_HANDLE_INDEX(index);

		SHandleSlot& slot = m_handleSlots[ index ];
		if (	slot.m_pData // slot is not free
				||slot.m_nextSlot == 0 ) // last free slot, cant be filled
		{
			return false; 
		}

		if ( SwapSlots( index, m_firstFreeSlot ) )
		{
			slot.m_pData = &data;
			slot.m_descendant = HandleToDescendant( handle );

			m_firstFreeSlot = slot.m_nextSlot;

			return true;
		}
		else
		{
			return false;
		}
	}

	Handle AddData( Type& data )
	{
		HandleIndexType freeSlotIndex = m_firstFreeSlot;
		SHandleSlot& freeSlot = m_handleSlots[ freeSlotIndex ];
		if ( freeSlot.m_nextSlot == 0 )
		{
			return 0; // last free slot, cant be filled
		}

		freeSlot.m_pData = &data;
		m_firstFreeSlot = freeSlot.m_nextSlot;

		return BuildHandle( freeSlotIndex, freeSlot.m_descendant );
	}

	CSmartPointer< Type > RemoveData( Handle handle )
	{
		HandleIndexType index = HandleIndexType(handle);
		CLAMP_HANDLE_INDEX(index);

		SHandleSlot& slot = m_handleSlots[ index ];
		if ( !slot.m_pData )
		{
			return NULL; // slot is already free
		}

		// swap slot to delete and last used slot, so that the slot to the delete is at the end of used slots list
		if ( SwapSlots( index, m_handleSlots[ m_firstFreeSlot ].m_prevSlot ) )
		{
			CSmartPointer< Type > pData = slot.m_pData;
			slot.m_pData = NULL;
			++slot.m_descendant; // increment descendant so that next handle with that index is different

			m_firstFreeSlot = index;
			
			return pData;			
		}
		else
		{
			return NULL;
		}		
	}

private:
	bool SwapSlots( HandleIndexType index1, HandleIndexType index2 )
	{
		if ( index1 == index2 )
		{
			return true;
		}

		if ( index1 == 0 || index2 == 0 )
		{
			return false;
		}

		CLAMP_HANDLE_INDEX(index1);
		CLAMP_HANDLE_INDEX(index2);
		SHandleSlot& slot1 = m_handleSlots[ index1 ];
		SHandleSlot& slot2 = m_handleSlots[ index2 ];

		bool bSlot1Free = slot1.m_pData;
		bool bSlot2Free = slot2.m_pData;
	
		if ( bSlot1Free != bSlot2Free )
		{
			return false; // Can only swap 2 free slots or 2 filled slots (since filled and free slots must be sorted)
		}

		HandleIndexType slot1Links[2] = { slot2.m_prevSlot, slot2.m_nextSlot };
		HandleIndexType slot2Links[2] = { slot1.m_prevSlot, slot1.m_nextSlot };

		if ( slot1Links[0] == index1 ) slot1Links[0] = index2;
		if ( slot1Links[1] == index1 ) slot1Links[1] = index2;
		if ( slot2Links[0] == index2 ) slot2Links[0] = index1;
		if ( slot2Links[1] == index2 ) slot2Links[1] = index1;

		slot1.m_prevSlot = slot1Links[0];
		if ( slot1Links[0] )
		{
			m_handleSlots[ slot1Links[0] ].m_nextSlot = index1;			
		}
		slot1.m_nextSlot = slot1Links[1];
		if ( slot1Links[1] )
		{
			m_handleSlots[ slot1Links[1] ].m_prevSlot = index1;
		}

		slot2.m_prevSlot = slot2Links[0];
		if ( slot2Links[0] )
		{
			m_handleSlots[ slot2Links[0] ].m_nextSlot = index2;
		}
		slot2.m_nextSlot = slot2Links[1];
		if ( slot2Links[1] )
		{
			m_handleSlots[ slot2Links[1] ].m_prevSlot = index2;
		}

		if ( index1 == m_firstFreeSlot )
		{
			m_firstFreeSlot = index2;
		}
		else if ( index2 == m_firstFreeSlot )
		{
			m_firstFreeSlot = index1;
		}

		return true;
	}


	struct SHandleSlot
	{
		HandleIndexType	m_descendant;	
		HandleIndexType	m_prevSlot;
		HandleIndexType	m_nextSlot;
		CSmartPointer< Type >	m_pData;
	};

	void Reset()
	{
		for( size_t iSlot=0 ; iSlot < Size ; ++iSlot )
		{
			SHandleSlot& slot = m_handleSlots[ iSlot ];
			slot.m_descendant = 0;
			slot.m_prevSlot = iSlot - 1;
			slot.m_nextSlot = iSlot + 1;
			slot.m_pData = NULL;
		}

		m_handleSlots[ 0 ].m_prevSlot = 0;
		m_handleSlots[ 0 ].m_nextSlot = 0;
		m_handleSlots[ Size - 1 ].m_nextSlot = 0;
		m_firstFreeSlot = 1; // 0 slot is invalid by default
	}

	SHandleSlot	m_handleSlots[ Size ];
	HandleIndexType	m_firstFreeSlot;
};

WHITEBOX_END

#endif
