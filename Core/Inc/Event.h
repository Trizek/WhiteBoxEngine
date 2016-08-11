#ifndef __EVENT_H__
#define __EVENT_H__

#include "BaseTypes.h"

//#define DECLARE_EVENT(TEvent) std::vector< WhiteBox::CEvent< TEvent >::SListenerInfo > WhiteBox::CEvent< TEvent >::s_listeners


WHITEBOX_BEGIN

template< class TEvent >
class CEvent
{
private:
	typedef void (*TStaticCallbackFunc)( void* pListener, const TEvent& event );
	
	struct SListenerInfo
	{
		SListenerInfo()
			: m_pListener(NULL), m_callbackFunc(NULL){}
			
		SListenerInfo( void* pListener, TStaticCallbackFunc callbackFunc )
			: m_pListener(pListener), m_callbackFunc(callbackFunc){}
			
		bool operator==( const void* pListener ) const
		{
			return (m_pListener == pListener);
		}
	
		void*	m_pListener;
		TStaticCallbackFunc	m_callbackFunc;
	};
	
	typedef std::vector< SListenerInfo > TListenerArray;
	
	template< class TListener, void (TListener::*MemberCallbackFunc)(const TEvent& event) >
	static void StaticCallBack( void* pListener, const TEvent& event )
	{
		if ( pListener )
		{
			TListener& listener = *static_cast< TListener* >( pListener );
			(listener.*MemberCallbackFunc)( event );
		}
	}
		
	static TListenerArray& GetListeners()
	{
		static TListenerArray s_listeners;
		return s_listeners;
	}
	
public:
	static void Send( const TEvent& event )
	{
		TListenerArray& listeners = GetListeners();
		
		for( size_t iListener=0 ; iListener < listeners.size() ; ++iListener )
		{
			SListenerInfo& listenerInfo = listeners[ iListener ];
			listenerInfo.m_callbackFunc( listenerInfo.m_pListener, event );
		}
	}

	class CObserver
	{
	public:
		CObserver( void* pListener )
			: m_pListener(pListener)
			, m_bBound(false){}
			
		~CObserver()
		{
			Unbind();
		}
	
		template< class TListener, void (TListener::*MemberCallbackFunc)( const TEvent& event ) >
		void Bind()
		{
			Unbind();
		
			GetListeners().push_back( SListenerInfo( m_pListener, &StaticCallBack< TListener, MemberCallbackFunc > ) );
			m_bBound = true;
		}
		
		void Unbind()
		{
			if ( m_bBound )
			{
				TListenerArray& listeners = GetListeners();
				typename TListenerArray::iterator it = std::find( listeners.begin(), listeners.end(), m_pListener );
				if ( it != listeners.end() )
				{
					SListenerInfo& back = listeners.back();
					if ( m_pListener != back.m_pListener )
					{
						*it = back;
					}
					listeners.resize( listeners.size() - 1 );
				}
				
				m_bBound = false;
			}
		}
		
	private:
		void* m_pListener;
		bool	m_bBound;
	};	
};


// With key
template< class TEvent, class TKey >
class CEventKey
{
protected:
	CEventKey(){}
	CEventKey(const CEventKey& event){}

private:
	typedef void (*TStaticCallbackFunc)( void* pListener, const TEvent& event, const TKey& key );
	
	struct SListenerInfo
	{
		SListenerInfo()
			: m_pListener(NULL), m_callbackFunc(NULL){}
			
		SListenerInfo( void* pListener, TStaticCallbackFunc callbackFunc )
			: m_pListener(pListener), m_callbackFunc(callbackFunc){}
			
		bool operator==( const void* pListener ) const
		{
			return (m_pListener == pListener);
		}
	
		void*	m_pListener;
		TStaticCallbackFunc	m_callbackFunc;
	};
	
	typedef std::vector< SListenerInfo > TListenerArray;
	typedef Map< TKey, TListenerArray > TListenerMap;
	
	template< class TListener, void (TListener::*MemberCallbackFunc)( const TEvent& event, const TKey& key ) >
	static void StaticCallBack( void* pListener, const TEvent& event, const TKey& key )
	{
		if ( pListener )
		{
			TListener& listener = *static_cast< TListener* >( pListener );
			(listener.*MemberCallbackFunc)( event, key );
		}
	}
		
	static TListenerArray& GetListeners( const TKey& key )
	{
		static TListenerMap s_listeners;
		return s_listeners[ key ];
	}
	
	static TListenerArray& GetGlobalListeners()
	{
		static TListenerArray s_listeners;
		return s_listeners;
	}
	
public:
	static void Send( const TEvent& event, const TKey& key )
	{
		{
			TListenerArray& listeners = GetListeners( key );
			
			for( size_t iListener=0 ; iListener < listeners.size() ; ++iListener )
			{
				SListenerInfo& listenerInfo = listeners[ iListener ];
				listenerInfo.m_callbackFunc( listenerInfo.m_pListener, event, key );
			}
		}
		
		{
			TListenerArray& listeners = GetGlobalListeners();
			
			for( size_t iListener=0 ; iListener < listeners.size() ; ++iListener )
			{
				SListenerInfo& listenerInfo = listeners[ iListener ];
				listenerInfo.m_callbackFunc( listenerInfo.m_pListener, event, key );
			}
		}		
	}

	class CObserver
	{
	private:
		enum EBinding
		{
			eB_NotBound = 0,
			eB_BoundToKey,
			eB_BoundToAllKeys,
		};
	public:
		CObserver( void* pListener )
			: m_pListener(pListener)
			, m_binding(eB_NotBound){}
			
		~CObserver()
		{
			Unbind();
		}
	
		template< class TListener, void (TListener::*MemberCallbackFunc)( const TEvent& event, const TKey& key ) >
		void Bind( const TKey& key )
		{
			Unbind();
		
			GetListeners( key ).push_back( SListenerInfo( m_pListener, &StaticCallBack< TListener, MemberCallbackFunc > ) );
			m_key = key;
			m_binding = eB_BoundToKey;
		}
		
		template< class TListener, void (TListener::*MemberCallbackFunc)( const TEvent& event, const TKey& key ) >
		void BindToAllKeys()
		{
			Unbind();
		
			GetGlobalListeners().push_back( SListenerInfo( m_pListener, &StaticCallBack< TListener, MemberCallbackFunc > ) );
			m_binding = eB_BoundToAllKeys;
		}		
		
		void Unbind()
		{
			if ( m_binding != eB_NotBound )
			{
				TListenerArray& listeners = (m_binding == eB_BoundToKey)? GetListeners( m_key ) : GetGlobalListeners();
				typename TListenerArray::iterator it = std::find( listeners.begin(), listeners.end(), m_pListener );
				if ( it != listeners.end() )
				{
					SListenerInfo& back = listeners.back();
					if ( m_pListener != back.m_pListener )
					{
						*it = back;
					}
					listeners.resize( listeners.size() - 1 );
				}
				
				m_binding = eB_NotBound;
			}
		}
		
	private:
		void* m_pListener;
		TKey m_key;
		EBinding m_binding;
	};	
};


WHITEBOX_END

#endif
