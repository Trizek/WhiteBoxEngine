#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

WHITEBOX_BEGIN

template< size_t BitCount >
class CBitSet
{
public:
	static const size_t BitFlagArraySize = BitCount / (sizeof(int) * 8) + 1;
	
	CBitSet()
	{
		Reset();
	}
	
	void Reset()
	{
		for( size_t iFlag=0 ; iFlag < BitFlagArraySize ; ++iFlag )
		{
			m_bitFlagArray[ iFlag ] = 0;
		}
	}
	
	bool	GetBit( size_t bitIndex ) const
	{
		return ( (m_bitFlagArray[ GetBitFlagIndex(bitIndex) ] & GetBitFlagMask(bitIndex)) != 0 ); 
	}
	
	void	SetBit( size_t bitIndex, bool bValue )
	{
		int bitMask = GetBitFlagMask(bitIndex);
		if ( bValue )
		{
			 m_bitFlagArray[ GetBitFlagIndex(bitIndex) ] |= bitMask; 
		}
		else
		{
			 m_bitFlagArray[ GetBitFlagIndex(bitIndex) ] &= ~bitMask;
		}
	}
	
	CBitSet< BitCount > Xor( const CBitSet< BitCount >& bitSet )
	{
		CBitSet< BitCount > res;
	
		for( size_t iFlag=0 ; iFlag < BitFlagArraySize ; ++iFlag )
		{
			res.m_bitFlagArray[ iFlag ] = m_bitFlagArray[ iFlag ] ^ bitSet.m_bitFlagArray[ iFlag ];
		}
		
		return res;
	}
	
private:
	static inline size_t	GetBitFlagIndex( size_t bitIndex )
	{
		size_t index = bitIndex / (sizeof(int) * 8);
		return (index < BitFlagArraySize)? index : BitFlagArraySize-1;
	}
	
	static inline int		GetBitFlagMask( size_t bitIndex )
	{
		return (int)(1 << (bitIndex % (sizeof(int) * 8)));
	}	

	int	m_bitFlagArray[ BitFlagArraySize ];
};


class CInputManager
{
public:
	enum EButtonId
	{
		eB_Key_A = 0,
		eB_Key_B,
		eB_Key_J,
		eB_Key_K,
		
		eB_ButtonCount,
	};

	bool IsButtonPressed( EButtonId buttonId ) const
	{
		return m_curButtonsState.GetBit( buttonId );
	}

	bool IsButtonJustPressed( EButtonId buttonId ) const
	{
		return m_curButtonsState.GetBit( buttonId ) && m_curButtonsStateChange.GetBit( buttonId );
	}

	bool IsButtonJustReleased( EButtonId buttonId ) const
	{
		return !m_curButtonsState.GetBit( buttonId ) && m_curButtonsStateChange.GetBit( buttonId );
	}

	// Called by rendering API, buffer input changed (so can be call at any time, safe)
	void UpdateButtonState( EButtonId buttonId, bool bPressed )
	{
		m_nextButtonsState.SetBit( buttonId, bPressed );
	}
	
	// Applied input changes happening since last call of the function
	void Poll()
	{
		// Check difference with prev states to notice just pressed buttons
		m_curButtonsStateChange = m_curButtonsState.Xor( m_nextButtonsState );
		
		// Apply to current changes
		m_curButtonsState = m_nextButtonsState;
		
		// Reset buffer
		m_nextButtonsState.Reset();
	}
	
private:	
	typedef CBitSet< eB_ButtonCount >	TButtonsState;
	
	TButtonsState	m_curButtonsState;
	TButtonsState	m_nextButtonsState;
	TButtonsState	m_curButtonsStateChange; // for just pressed buttons
};




WHITEBOX_END

#endif
