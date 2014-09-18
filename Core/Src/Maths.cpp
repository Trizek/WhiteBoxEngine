#include "BaseTypes.h"


WHITEBOX_BEGIN


Degree::Degree( float _angle )
	: angle(_angle){}

Degree::Degree( const Radian& radian )
	: angle( radian.angle / DEG2RAD ){}
	
Degree Degree::operator*( float coeff ) const
{
	return Degree( angle * coeff );
}


Radian::Radian( float _angle )
	: angle(_angle){}

Radian::Radian( const Degree& degree )
	: angle( degree.angle * DEG2RAD ){}
	
Radian Radian::operator*( float coeff ) const
{
	return Radian( angle * coeff );
}


float Cos( Radian angle )
{
	return cosf( angle.angle );
}

float Sin( Radian angle )
{
	return sinf( angle.angle );
}

float Tan( Radian angle )
{
	return tanf( angle.angle );
}

Vec3 operator*( const float scalar, const Vec3& rhs )
{
	return Vec3( scalar*rhs.x, scalar*rhs.y, scalar*rhs.z );
}

Vec3 operator-( const Vec3& rhs )
{
	return Vec3( -rhs.x, -rhs.y, -rhs.z );
}

Quat operator!( const Quat& q )
{
	return q.getInverse();
}

Transform operator!( const Transform& t )
{
	return t.getInverse();
}

const Color Color::Black = Color( 0.0f, 0.0f, 0.0f );
const Color Color::White = Color( 1.0f, 1.0f, 1.0f );
const Color Color::Red = Color( 1.0f, 0.0f, 0.0f );
const Color Color::Green = Color( 0.0f, 1.0f, 0.0f );
const Color Color::Blue = Color( 0.0f, 0.0f, 1.0f );

WHITEBOX_END