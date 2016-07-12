#ifndef __BASETYPES_H__
#define __BASETYPES_H__

#define BIT( n ) (1 << (n)) 

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

#include <vector>

#include "WhiteBoxString.h"

#ifdef __APPLE__

#include <ext/hash_map>
WHITEBOX_BEGIN

template< class Key, class Value >
class Map : public __gnu_cxx::hash_map< Key, Value >
{
};

WHITEBOX_END

#elif defined( _WIN64 ) || defined( _WIN32 )

#include <unordered_map>
WHITEBOX_BEGIN


template< class Key, class Value >
class Map : public std::unordered_map< Key, Value >
{
};

WHITEBOX_END

#endif


/*
#include <ext/hash_map>


namespace __gnu_cxx
{
using namespace std;

template<>
struct hash<WhiteBox::String>
{
size_t operator()(const WhiteBox::String& s) const
{
return 0;
}
};
}

*/

#include "SmartPointer.h"

WHITEBOX_BEGIN
#include "Maths.h"
WHITEBOX_END

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

#endif