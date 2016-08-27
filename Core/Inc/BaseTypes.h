#ifndef __BASETYPES_H__
#define __BASETYPES_H__

#define BIT( n ) (1 << (n)) 

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

#if defined( _WIN64 ) || defined( _WIN32 )

typedef char32_t wchar;

#else

#include <wchar.h>
typedef char32_t wchar;

#endif

#include <vector>

#include "Types/HashMap.h"
#include "WhiteBoxString.h"
#include "Text.h"


#include "SmartPointer.h"

#include "LogSystem/LogSystem.h"

WHITEBOX_BEGIN
#include "Maths.h"


enum class Key : unsigned int
{
	F1 = 0,
	F2,
	F3,
	F4,
	F5,

	Z,
	Q,
	S,
	D,

	Count,
};

enum class ETouchEventType
{
	Begin = 0,
	Stay,
	End,
};

struct STouchEvent
{
	ETouchEventType		eventType;
	Vec2				pos;
};

WHITEBOX_END


#endif
