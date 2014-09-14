#ifndef __MATHS_H__
#define __MATHS_H__

#include <math.h>


#define PI 3.14f

struct Radian;
struct Degree
{
	explicit Degree( float _angle );
	Degree( const Radian& radian ); 

	float angle;
};

struct Radian
{
	explicit Radian( float _angle );
	Radian( const Degree& degree ); 

	float angle;
};


struct Vec4
{
	Vec4()
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{}
	
	Vec4( float val )
		: x(val), y(val), z(val), w(val)
	{}
	
	Vec4( float _x, float _y, float _z, float _w )
		: x(_x), y(_y), z(_z), w(_w)
	{}
	
	size_t	GetHashCode() const
	{
		size_t hashCode = (size_t)x + (size_t)y + (size_t)z + (size_t)w;
		return hashCode;
	}

	float x, y, z, w;
};

struct Vec3
{
	Vec3()
		: x(0.0f), y(0.0f), z(0.0f)
	{}
	
	Vec3( float val )
		: x(val), y(val), z(val)
	{}
	
	Vec3( float _x, float _y, float _z )
		: x(_x), y(_y), z(_z)
	{}
	
	Vec3( const Vec3& rhs ) : x(rhs.x), y(rhs.y), z(rhs.z){}	
	
	size_t	GetHashCode() const
	{
		size_t hashCode = (size_t)x + (size_t)y + (size_t)z;
		return hashCode;
	}
	
	Vec3 operator+( const Vec3& rhs ) const
	{
		return Vec3( x + rhs.x, y + rhs.y, z + rhs.z );
	}
	
	Vec3 operator-( const Vec3& rhs ) const
	{
		return Vec3( x - rhs.x, y - rhs.y, z - rhs.z );
	}
	
	Vec3& operator+=( const Vec3& rhs )
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	
	Vec3 operator*( const float scalar ) const
	{
		return Vec3( scalar*x, scalar*y, scalar*z );
	}

	Vec3 operator/( const float scalar ) const
	{
		return Vec3( x/scalar, y/scalar, z/scalar );
	}	
	
	float operator|( const Vec3& rhs ) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}
	
	Vec3 operator*( const Vec3& rhs ) const
	{
		return Vec3( y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x );
	}
	
	float Length() const
	{
		return sqrtf( x*x + y*y + z*z );
	}
	
	float SqrLength() const
	{
		return x*x + y*y + z*z;
	}
	
	void Normalize()
	{
		if ( x || y || z )
		{
			float length = Length();
			x /= length;
			y /= length;
			z /= length;
		}
	}	

	float x, y, z;
};

struct Vec2
{
	Vec2()
		: x(0.0f), y(0.0f)
	{}
	
	Vec2( float val )
		: x(val), y(val)
	{}
	
	Vec2( float _x, float _y )
		: x(_x), y(_y)
	{}
	
	size_t	GetHashCode() const
	{
		size_t hashCode = (size_t)x + (size_t)y;
		return hashCode;
	}

	float x, y;
};

/*
Vec3 operator*( const float scalar, const Vec3& rhs )
{
	return Vec3( scalar*rhs.x, scalar*rhs.y, scalar*rhs.z );
}*/

/*
Vec3 operator-( const Vec3& rhs )
{
	return Vec3( -rhs.x, -rhs.y, -rhs.z );
}*/

// in fact it's a Matrix4x4 but the last line is (0,0,0,1)
// M = (V1 V2 V3 T), Rotation = Axis(V1,V2,V3), Translation=T;
struct Matrix34
{
	Matrix34() // identity
	{
		a11=1.0f; a21=0.0f, a31=0.0f; // V1
		a12=0.0f; a22=1.0f, a32=0.0f; // V2
		a13=0.0f; a23=0.0f, a33=1.0f; // V3
		a14=0.0f; a24=0.0f, a34=0.0f; // T
	}

	float a11, a12, a13, a14;
	float a21, a22, a23, a24;
	float a31, a32, a33, a34;
	
	Matrix34 GetInvertedOrtho() const
	{
		// (V1',V2'V3') = T(V1,V2,V3)
		Matrix34 res;
		// V1'
		res.a11 = a11;
		res.a21 = a12;
		res.a31 = a13;
		// V2'
		res.a12 = a21;
		res.a22 = a22;
		res.a32 = a23;	
		// V3'
		res.a13 = a31;
		res.a23 = a32;
		res.a33 = a33;
		// T' = T(V1,V2,V3) * T = (V1.T, V2.T, V3.T)
		res.a14 = a11*a14 + a21*a24 + a31*a34;
		res.a24 = a12*a14 + a22*a24 + a32*a34;
		res.a34 = a13*a14 + a23*a24 + a33*a34;
		
		return res;
	}
	
	Matrix34 operator*( const Matrix34& rhs ) const
	{
		Matrix34 res;
		// V1
		res.a11 = a11*rhs.a11 + a12*rhs.a21 + a13*rhs.a31;
		res.a21 = a21*rhs.a11 + a22*rhs.a21 + a23*rhs.a31;
		res.a31 = a31*rhs.a11 + a32*rhs.a21 + a33*rhs.a31;
		// V2
		res.a12 = a11*rhs.a12 + a12*rhs.a22 + a13*rhs.a32;
		res.a22 = a21*rhs.a12 + a22*rhs.a22 + a23*rhs.a32;
		res.a32 = a31*rhs.a12 + a32*rhs.a22 + a33*rhs.a32;		
		// V3
		res.a13 = a11*rhs.a13 + a12*rhs.a23 + a13*rhs.a33;
		res.a23 = a21*rhs.a13 + a22*rhs.a23 + a23*rhs.a33;
		res.a33 = a31*rhs.a13 + a32*rhs.a23 + a33*rhs.a33;
		// T
		res.a14 = a11*rhs.a14 + a12*rhs.a24 + a13*rhs.a34 + a14;	
		res.a24 = a21*rhs.a14 + a22*rhs.a24 + a23*rhs.a34 + a24;
		res.a34 = a31*rhs.a14 + a32*rhs.a24 + a33*rhs.a34 + a34;	
	}
	
	Vec3 operator*( const Vec3& v ) const
	{
		Vec3 res(	a11*v.x + a12*v.y + a13*v.z + a14,
					a21*v.x + a22*v.y + a23*v.z + a24,
					a31*v.x + a32*v.y + a33*v.z + a34 );
					
		return res;
	};
	
	Vec3 Rotate( const Vec3& v )
	{
		Vec3 res(	a11*v.x + a12*v.y + a13*v.z,
					a21*v.x + a22*v.y + a23*v.z,
					a31*v.x + a32*v.y + a33*v.z );
	};	
	
	Vec3 GetAxisX() const
	{
		return Vec3( a11, a21, a31 );
	}
	
	Vec3 GetAxisY() const
	{
		return Vec3( a12, a22, a32 );
	}
	
	Vec3 GetAxisZ() const
	{
		return Vec3( a12, a22, a32 );
	}
	
	Vec3 GetTranslation() const
	{
		return Vec3( a14, a24, a34 );
	}
};

struct Matrix44
{
	Matrix44() // identity
	{
		a11=1.0f; a21=0.0f, a31=0.0f; a41 = 0.0f;
		a12=0.0f; a22=1.0f, a32=0.0f; a42 = 0.0f;
		a13=0.0f; a23=0.0f, a33=1.0f; a43 = 0.0f;
		a14=0.0f; a24=0.0f, a34=0.0f; a44 = 1.0f;
	}

	float a11, a12, a13, a14; // Col 1
	float a21, a22, a23, a24; // Col 2
	float a31, a32, a33, a34; // Col 3
	float a41, a42, a43, a44; // Col 4
};

#endif