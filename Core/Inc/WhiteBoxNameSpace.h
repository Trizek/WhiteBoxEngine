#ifndef __WHITEBOXNAMESPACE_H__
#define __WHITEBOXNAMESPACE_H__

#define	WHITEBOX_BEGIN namespace WhiteBox{\

#define WHITEBOX_END };


template< class Type, bool bConst >
class ReferenceStatic
{
public:
	typedef int Ref;


// 	template< bool bConst >
// 	class  Const
// 	{
// 	public:
// 		typedef int Ref;
// 	};
// 
// 	template<>
// 	class Const< true >
// 	{
// 	public:
// 		typedef typename const Type& Ref;
// 	};
// 
// 	template<>
// 	class Const< false >
// 	{
// 	public:
// 		typedef typename Type& Ref;
// 	};
};

template< class Type >
class ReferenceStatic< Type, true >
{
public:
	typedef const Type& Ref;
};

template< class Type >
class ReferenceStatic< Type, false >
{
public:
	typedef Type& Ref;
};




template< class Type, bool bConst >
class Reference : public ReferenceStatic< Type, bConst >
{};

#define REFERENCE( Type, bConst ) Reference< Type, bConst >::Ref

#endif