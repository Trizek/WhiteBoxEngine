#ifndef __WHITEBOXNAMESPACE_H__
#define __WHITEBOXNAMESPACE_H__

#define	WHITEBOX_BEGIN namespace WhiteBox{\

#define WHITEBOX_END };

template< class Type >
class ReferenceStatic
{
public:
	template< bool bConst >
	class  Const
	{
	public:
		typedef typename int Ref;
	};

	template<>
	class Const< true >
	{
	public:
		typedef typename const Type& Ref;
	};

	template<>
	class Const< false >
	{
	public:
		typedef typename Type& Ref;
	};
};

template< class Type, bool bConst >
class Reference : public ReferenceStatic< Type >::Const< bConst >
{};

#define REFERENCE( Type, bConst ) Reference< Type, bConst >::Ref

#endif