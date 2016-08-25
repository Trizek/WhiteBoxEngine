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

Vec3 Vec3::Zero = Vec3(0.0f, 0.0f, 0.0f);
Vec3 Vec3::Forward = Vec3(0.0f, 1.0f, 0.0f);
Vec3 Vec3::Backward = Vec3(0.0f, -1.0f, 0.0f);
Vec3 Vec3::Left = Vec3(-1.0f, 0.0f, 0.0f);
Vec3 Vec3::Right = Vec3(1.0f, 0.0f, 0.0f);
Vec3 Vec3::Down = Vec3(0.0f, 0.0f, -1.0f);
Vec3 Vec3::Up = Vec3(0.0f, 0.0f, 1.0f);


Vec3 operator*( const float scalar, const Vec3& rhs )
{
	return Vec3( scalar*rhs.x, scalar*rhs.y, scalar*rhs.z );
}

Vec3 operator-( const Vec3& rhs )
{
	return Vec3( -rhs.x, -rhs.y, -rhs.z );
}

Transform Transform::Zero = Transform( Vec3::Zero, Quat(0.0f, 0.0f, 0.0f, 0.0f), Vec3::Zero );

Transform Transform::operator*( const Transform& t ) const
{
	Transform res;
	res.position = position + rotation * t.position;
	res.rotation = rotation * t.rotation;
	res.scale = Vec3( scale.x*t.scale.x, scale.y*t.scale.y, scale.z*t.scale.z );		
	return res;
}

Transform Transform::operator+( const Transform& t ) const
{
	return Transform( position + t.position, rotation + t.rotation );
}


ushort	Quat::EncodeComponent( float component )
{
	static short start = (short)(1 << (sizeof(short)*8-1));
	static short end = start - 1;
	
	short val = (ushort)( (0.5f + component * 0.5f)*int((int)end - (int)start) );
	return val;
}

float	Quat::DecodeComponent( ushort val )
{
	static short start = (short)(1 << (sizeof(short)*8-1));
	static short end = start - 1;

	float component = -1.0f + (float(val) / float((int)end - (int)start)) * 2.0f;
	return component;	
}
	
void	Quat::EncodeQuat( const Quat& q, ushort& s0, ushort& s1, ushort& s2 )
{
	if ( q.w < 0.0f )
	{
		s0 = EncodeComponent( -q.x );
		s1 = EncodeComponent( -q.y );
		s2 = EncodeComponent( -q.z );
	}
	else
	{
		s0 = EncodeComponent( q.x );
		s1 = EncodeComponent( q.y );
		s2 = EncodeComponent( q.z );	
	}
}

void	Quat::DecodeQuat( ushort s0, ushort s1, ushort s2, Quat& q )
{
	q.x = DecodeComponent( s0 );
	q.y = DecodeComponent( s1 );
	q.z = DecodeComponent( s2 );
	q.w = sqrtf( 1.0f - ( q.x*q.x + q.y*q.y + q.z*q.z ) );
}


Quat operator!( const Quat& q )
{
	return q.getInverse();
}

Quat operator*( float factor, const Quat& q )
{
	return Quat( factor * q.w, factor * q.x, factor * q.y, factor * q.z );
}

Transform operator!( const Transform& t )
{
	return t.GetInverse();
}

Transform operator*( float factor, const Transform& t )
{
	return Transform( factor * t.position, factor * t.rotation, factor * t.scale );
}

const Color Color::Black = Color( 0.0f, 0.0f, 0.0f );
const Color Color::White = Color( 1.0f, 1.0f, 1.0f );
const Color Color::Red = Color( 1.0f, 0.0f, 0.0f );
const Color Color::Green = Color( 0.0f, 1.0f, 0.0f );
const Color Color::Blue = Color( 0.0f, 0.0f, 1.0f );

WHITEBOX_END