#ifndef __MATHS_H__
#define __MATHS_H__

#include <math.h>


template< class T >
T	Min( const T& a, const T& b )
{
	return ( a < b )? a : b;
}

template< class T >
T	Max( const T& a, const T& b )
{
	return ( a > b )? a : b;
}

template< class T >
T	Clamp( const T& x, const T& a, const T& b )
{
	return Min( Max( x, a ), b );
}

#define PI 3.1419f
#define DEG2RAD (PI/180.0f)

struct Radian;
struct Degree
{
	explicit Degree( float _angle );
	Degree( const Radian& radian );
	
	Degree operator*( float coeff ) const;

	float angle;
};

struct Radian
{
	explicit Radian( float _angle );
	Radian( const Degree& degree ); 
	
	Radian operator*( float coeff ) const;

	float angle;
};

float Cos( Radian angle );
float Sin( Radian angle );
float Tan( Radian angle );


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
	static Vec3 Zero;
	static Vec3 Forward;
	static Vec3 Backward;
	static Vec3 Left;
	static Vec3 Right;
	static Vec3 Down;
	static Vec3 Up;
	

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
	
	bool IsNull( float epsilon = 0.01f ) const
	{
		return ( SqrLength() <= epsilon * epsilon );
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
	
	Vec2 operator-( const Vec2& rhs ) const
	{
		return Vec2( x - rhs.x, y - rhs.y );
	}

	size_t	GetHashCode() const
	{
		size_t hashCode = (size_t)x + (size_t)y;
		return hashCode;
	}

	float x, y;
};


Vec3 operator*( const float scalar, const Vec3& rhs );
Vec3 operator-( const Vec3& rhs );

// (x,y,z) = u sin(angle/2), w = cos(angle/2)
struct Quat
{
	Quat()
		: w(1.0f), x(0.0f), y(0.0f), z(0.0f){}
		
	Quat( float _w, float _x, float _y, float _z )
		: w(_w), x(_x), y(_y), z(_z){}
		
	Quat( const Vec3& axis, Radian angle )
	{
		float cos = Cos( angle * 0.5f );
		float sin = Sin( angle * 0.5f );
		
		w = cos;
		x = axis.x * sin;
		y = axis.y * sin;
		z = axis.z * sin;
	}
	
	static Quat CreateRotX( Radian angle )
	{
		Quat q;
		q.w = Cos( angle * 0.5f );
		q.x = Sin( angle * 0.5f );
		return q;
	}
	
	static Quat CreateRotY( Radian angle )
	{
		Quat q;
		q.w = Cos( angle * 0.5f );
		q.y = Sin( angle * 0.5f );
		return q;
	}
	
	static Quat CreateRotZ( Radian angle )
	{
		Quat q;
		q.w = Cos( angle * 0.5f );
		q.z = Sin( angle * 0.5f );
		return q;
	}
	
	static ushort	EncodeComponent( float component );
	static float	DecodeComponent( ushort val );
	
	static void		EncodeQuat( const Quat& q, ushort& s0, ushort& s1, ushort& s2 );
	static void		DecodeQuat( ushort s0, ushort s1, ushort s2, Quat& q );
		
	Quat( Radian yaw, Radian pitch, Radian roll )
	{
		Quat q = CreateRotZ( yaw ) * CreateRotX( pitch ) * CreateRotY( roll );
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
	}
	
	bool IsNull( float epsilon = 0.01f ) const
	{
		return ( SqrLength() <= epsilon * epsilon );
	}
	
	float Length() const
	{
		return sqrtf( w*w + x*x + y*y + z*z );
	}
	
	float SqrLength() const
	{
		return w*w + x*x + y*y + z*z;
	}
	
	void Normalize()
	{
		if ( w || x || y || z )
		{
			float length = Length();

			w /= length;
			x /= length;
			y /= length;
			z /= length;
		}
	}	
	
	Quat operator+( const Quat& q ) const
	{
		Quat res( w + q.w, x + q.x, y + q.y, z + q.z );
		return res;
	}
	
	Quat operator*( const Quat& q ) const
	{
		Quat res;
		res.w = w*q.w - x*q.x - y*q.y - z*q.z;
		res.x = w*q.x + x*q.w + y*q.z - z*q.y;
		res.y = w*q.y - x*q.z + y*q.w + z*q.x;
		res.z = w*q.z + x*q.y - y*q.x + z*q.w;
		
		return res; 
	}
	
	float	operator|( const Quat& q ) const
	{
		return w*q.w + x*q.x + y*q.y + z*q.z;
	}

	Quat getInverse() const
	{
		return Quat( -w, x, y, z );
	}
	
	Vec3 operator*( const Vec3& v ) const
	{
		Vec3 res;
		res.x =	(1.0f - 2.0f*y*y - 2.0f*z*z) * v.x +
				(2.0f*x*y - 2.0f*z*w) * v.y +
				(2.0f*x*z + 2.0f*y*w) * v.z;
				
		res.y =	(2.0f*x*y + 2.0f*z*w) * v.x +
				(1.0f - 2.0f*x*x - 2.0f*z*z) * v.y +
				(2.0f*y*z - 2.0f*x*w) * v.z;
				
		res.z =	(2.0f*x*z - 2.0f*y*w) * v.x +
				(2.0f*y*z + 2.0f*x*w) * v.y +
				(1.0f - 2.0f*x*x - 2.0f*y*y) * v.z;
				
		return res;
	}
	


	float w, x, y, z;
};

Quat operator!( const Quat& q );
Quat operator*( float factor, const Quat& q );

struct Transform
{
	static Transform Zero;

	Transform()
		: scale(1.0f){}
		
	Transform( const Vec3& _position, const Quat& _rotation )
		: position(_position)
		, rotation(_rotation)
		, scale(1.0f){}

	Transform( const Vec3& _position, const Quat& _rotation, const Vec3& _scale )
		: position(_position)
		, rotation(_rotation)
		, scale(_scale){}
		
	Transform operator+( const Transform& t ) const;

	Transform operator*(const Transform& t) const;
	
	Vec3 operator*( const Vec3& v ) const
	{
		return position + rotation * Vec3( v.x * scale.x, v.y * scale.y, v.z * scale.z );
	}

	Vec3 TransformVector( const Vec3& v ) const
	{
		return rotation * v;
	}

	void Normalize()
	{
		rotation.Normalize();
	}
	
	Transform GetInverse() const
	{
		Transform res;
		res.position = !rotation * (-position);
		res.rotation = !rotation;
		res.scale = Vec3( 1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z );
		
		return res;
	}
		

	Vec3	position;
	Quat	rotation;
	Vec3	scale;
};

Transform operator!( const Transform& t );
Transform operator*( float factor, const Transform& t );

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

	float a11, a21, a31;  // X
	float a12, a22, a32;  // Y
	float a13, a23, a33;  // Z
	float a14, a24, a34;  // T

	/*float a11, a12, a13, a14;
	float a21, a22, a23, a24;
	float a31, a32, a33, a34;
*/
	void	FromTransform(const Transform& transform)
	{
		Vec3 Forward = transform.rotation * Vec3::Forward;
		Vec3 Right = transform.rotation * Vec3::Right;
		Vec3 Up = transform.rotation * Vec3::Up;

		SetAxisX( Right );
		SetAxisY( Forward );
		SetAxisZ( Up );
		Scale( transform.scale );

		SetTranslation( transform.position );
	}
	
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

		return res;
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

		return res;
	};	
	
	Vec3 GetScaledAxisX() const
	{
		return Vec3( a11, a21, a31 );
	}
	
	Vec3 GetScaledAxisY() const
	{
		return Vec3( a12, a22, a32 );
	}
	
	Vec3 GetScaledAxisZ() const
	{
		return Vec3( a13, a23, a33 );
	}
	
	Vec3 GetTranslation() const
	{
		return Vec3( a14, a24, a34 );
	}



	void SetAxisX( const Vec3& v )
	{
		a11 = v.x; a21 = v.y; a31 = v.z;
	}

	void SetAxisY( const Vec3& v )
	{
		a12 = v.x; a22 = v.y; a32 = v.z;
	}

	void SetAxisZ( const Vec3& v )
	{
		a13 = v.x; a23 = v.y; a33 = v.z;
	}

 	void SetTranslation( const Vec3& v )
	{
		a14 = v.x; a24 = v.y; a34 = v.z;
	}

	void Scale( const Vec3& v )
	{
		a11 *= v.x; a21 *= v.x; a31 *= v.x;
		a12 *= v.y; a22 *= v.y; a32 *= v.y;
		a13 *= v.z; a23 *= v.z; a33 *= v.z;
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

	Matrix44(const Matrix34& m)
	{
		a11 = m.a11;
		a12 = m.a21;
		a13 = m.a31;
		a14 = 0.0f;

		// Up
		a21 = m.a12;
		a22 = m.a22;
		a23 = m.a32;
		a24 = 0.0f;

		// Backward
		a31 = m.a13;
		a32 = m.a23;
		a33 = m.a33;
		a34 = 0.0f;

		// Position
		a41 = m.a14;
		a42 = m.a24;
		a43 = m.a34;
		a44 = 1.0f;
	}

	float a11, a12, a13, a14; // Col 1
	float a21, a22, a23, a24; // Col 2
	float a31, a32, a33, a34; // Col 3
	float a41, a42, a43, a44; // Col 4

	Matrix44 operator*( const Matrix44& rhs ) const
	{
		Matrix44 res;
		// V1
		res.a11 = a11*rhs.a11 + a12*rhs.a21 + a13*rhs.a31 + a14*rhs.a41;
		res.a21 = a21*rhs.a11 + a22*rhs.a21 + a23*rhs.a31 + a24*rhs.a41;
		res.a31 = a31*rhs.a11 + a32*rhs.a21 + a33*rhs.a31 + a34*rhs.a41;
		res.a41 = a41*rhs.a11 + a42*rhs.a21 + a43*rhs.a31 + a44*rhs.a41;

		res.a12 = a11*rhs.a12 + a12*rhs.a22 + a13*rhs.a32 + a14*rhs.a42;
		res.a22 = a21*rhs.a12 + a22*rhs.a22 + a23*rhs.a32 + a24*rhs.a42;
		res.a32 = a31*rhs.a12 + a32*rhs.a22 + a33*rhs.a32 + a34*rhs.a42;
		res.a42 = a41*rhs.a12 + a42*rhs.a22 + a43*rhs.a32 + a44*rhs.a42;

		res.a13 = a11*rhs.a13 + a12*rhs.a23 + a13*rhs.a33 + a14*rhs.a43;
		res.a23 = a21*rhs.a13 + a22*rhs.a23 + a23*rhs.a33 + a24*rhs.a43;
		res.a33 = a31*rhs.a13 + a32*rhs.a23 + a33*rhs.a33 + a34*rhs.a43;
		res.a43 = a41*rhs.a13 + a42*rhs.a23 + a43*rhs.a33 + a44*rhs.a43;

		res.a14 = a11*rhs.a14 + a12*rhs.a24 + a13*rhs.a34 + a14*rhs.a44;
		res.a24 = a21*rhs.a14 + a22*rhs.a24 + a23*rhs.a34 + a24*rhs.a44;
		res.a34 = a31*rhs.a14 + a32*rhs.a24 + a33*rhs.a34 + a34*rhs.a44;
		res.a44 = a41*rhs.a14 + a42*rhs.a24 + a43*rhs.a34 + a44*rhs.a44;


		return res;
	}
};

struct Color
{
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f){}
	Color( float _r, float _g, float _b ) : r(_r), g(_g), b(_b), a(1.0f){}
	
	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;

	float r, g, b, a;
};

class Matrix22
{
	Matrix22()
		: a(0.0f), b(0.0f), c(0.0f), d(0.0f){}

	Matrix22( float _a, float _b, float _c, float _d )
		: a(_a), b(_b), c(_c), d(_d){}
		
	bool	Inverse( Matrix22& inverse ) const
	{
		float det = a*d - b*c;
		if ( det != 0.0f )
		{
			return false;
		}
		
		float coeff = 1.0f / det;
		inverse.a = coeff * d;
		inverse.b = -coeff * b;
		inverse.c = -coeff * c;
		inverse.d = coeff * a;

		return true;
	}
	
	Matrix22 operator*( const Matrix22& mat ) const
	{
		Matrix22 res;
		res.a = a*mat.a + b*mat.c;
		res.b = a*mat.b + b*mat.d;
		res.c = c*mat.a + d*mat.c;
		res.d = c*mat.b + d*mat.d;
		
		return res;
	}
	
	Vec2 operator*( const Vec2& v ) const
	{
		return Vec2( a*v.x + b*v.y, c*v.x + d*v.y );
	}

	float a, b, c, d;
};

Vec3	ProjectPointOnLine( const Vec3& point, const Vec3& linePoint, const Vec3& lineDir );
Vec3	ProjectLineOnLine( const Vec3& fromPoint, const Vec3& fromDir, const Vec3& toPoint, const Vec3& toDir );

#if 0

template< class T >
void LinearInterpolate( float t0, const T& P0, float t1, const T& P1, float t, T& interpolated )
{
	float w = (t - t0) / (t1 - t0);
	interpolated = (1.0f - w) * P0 + w * P1;
}

template< class T >
void CubicInterpolate( float t0, const T& P0, const T& P0Derivative, float t1, const T& P1, const T& P1Derivative, float t, T& interpolated )
{
	float t01 = t1 - t0;
	float t01Sqr = t01 * t01;
	float t01Cube = t01Sqr * t01;
	Matrix22 A( t01Cube, t01Sqr, 3.0f * t01Sqr, 2.0f * t01 );
	Matrix22 invA;
	A.Inverse( invA );
		
	T Bx = P1 - P0 - t01 * P0Derivative;
	T By = P1Derivative - P0Derivative;
	
	T a = invA.a * Bx + invA.b * By;
	T b = invA.c * Bx + invA.d * By;
	const T& c = P0Derivative;
	const T& d = P0;
	
	float t0t = t - t0;
	float t0tSqr = t0t * t0t;
	float t0tCube = t01Sqr * t0t;
	interpolated = t0tCube * a + t0tSqr * b + t0t * c + d;
}

template< class T >
void CubicInterpolate( float t0, const T& P0,
					float t1, const T& P1,
					float t2, const T& P2,
					float t3, const T& P3,
					float t, T& interpolated )
{
	T P1Derivative = (1.0f/(t2 - t0)) * (P2 - P0);
	T P2Derivative = (1.0f/(t3 - t1)) * (P3 - P1);
	
	CubicInterpolate( t1, P1, P1Derivative, t2, P2, P2Derivative, t, interpolated );
}

template< class T >
void QuadraticInterpolateDerivEnd( float t0, const T& P0, const T& P0Derivative, float t1, const T& P1, float t, T& interpolated )
{
	float t01 = t1 - t0;
	float t01Sqr = t01 * t01;
	T a = (P1 - P0 - t01 * P0Derivative) / t01Sqr;
	const T& b = P0Derivative;
	const T& c = P0;
	
	interpolated = t01Sqr * a + t01 * b + c;
}

template< class T >
void QuadraticInterpolateDerivBegin( float t0, const T& P0, float t1, const T& P1, const T& P1Derivative, float t, T& interpolated )
{
	QuadraticInterpolateDerivEnd( -t1, P1, P1Derivative, -t0, P0, -t, interpolated );
}

template< class T >
void QuadraticInterpolateBegin( float t0, const T& P0, float t1, const T& P1, float t2, const T& P2, float t, T& interpolated )
{
	T P1Derivative = (1.0f/(t2 - t0)) * (P2 - P0);
	QuadraticInterpolateDerivBegin( t0, P0, t1, P1, P1Derivative, t, interpolated );
}

template< class T >
void QuadraticInterpolateEnd( float t0, const T& P0, float t1, const T& P1, float t2, const T& P2, float t, T& interpolated )
{
	T P1Derivative = (1.0f/(t2 - t0)) * (P2 - P0);
	QuadraticInterpolateDerivEnd( t1, P1, P1Derivative, t2, P2, t, interpolated );
}

template< class T >
class CubicSpline
{
public:
	class CKey
	{
	public:
		struct SCompare
		{
			bool operator()( const CKey& a, const CKey& b ) const
			{
				return ( a.m_time < b.m_time );
			}
		};
		
		CKey()
			: m_time(0.0f){}
	
		CKey( float time, const T& val )
			: m_time(time), m_val(val){}
	
		float		m_time;
		T			m_val;
	};
	
	void	FindSurroundingKeys( float animTime, size_t& leftKey, size_t& rightKey ) const
	{
		for( size_t iKey = 0 ; iKey < m_keys.size() ; ++iKey )
		{
			float time = m_keyFrames[ iKey ].m_time;
			if ( time >= animTime )
			{
				if ( iKey == 0 )
				{
					leftKey = 0;
					rightKey = 1;
				}
				else
				{
					rightKey = iKey;
					leftKey = iKey - 1;
				}
				return;
			}
		}
		
		rightKey = m_keyFrames.size() - 1;
		leftKey = rightKey - 1;
	}
	
	bool	Empty() const
	{
		return m_keys.size() == 0;
	}
	
	float	GetLength() const
	{
		return (Empty())? 0.0f : m_keys.back().m_time;
	}
	
	void	AddKey( float time, const T& val )
	{
		m_keys.push_back( CKey( time, val ) );
		SortKeys();
	}
	
	void	SortKeys()
	{
		std::sort( m_keys.begin(), m_keys.end(), CKey::SCompare() );
	}
	
	T GetValue( float time ) const
	{
		if ( Empty() )
		{
			return T();
		}
	
		time = Clamp( time, 0.0f, GetLength() );
	
		if( m_keys.size() == 1 )
		{
			return m_keys[ 0 ].m_val;
		}
		
		T interpolated;
		if( m_keys.size() == 2 )
		{
			LinearInterpolate(
				m_keys[ 0 ].m_time,
				m_keys[ 0 ].m_val,
				m_keys[ 1 ].m_time,
				m_keys[ 1 ].m_val,
				animTime, interpolated );
		}
		else
		{
			size_t leftKey, rightKey;
			FindSurroundingKeys( time, leftKey, rightKey );
			
			if ( leftKey == 0 )
			{
				// First segment
				QuadraticInterpolateBegin( 
					m_keys[ leftKey ].m_time, 
					m_keys[ leftKey ].m_val, 
					m_keys[ rightKey ].m_time, 
					m_keys[ rightKey ].m_val,  
					m_keys[ rightKey + 1 ].m_time, 
					m_keys[ rightKey + 1 ].m_val,  
					animTime, interpolated );
			}
			else if ( rightKey == m_keyFrames.size() - 1 )
			{
				// Last segment
				QuadraticInterpolateEnd( 
					m_keys[ leftKey - 1 ].m_time, 
					m_keys[ leftKey - 1 ].m_val, 
					m_keys[ leftKey ].m_time, 
					m_keys[ leftKey ].m_val,
					m_keys[ rightKey ].m_time, 
					m_keys[ rightKey ].m_val,  
					animTime, interpolated );				
			}
			else
			{
				CubicInterpolate(
					m_keys[ leftKey - 1 ].m_time, 
					m_keys[ leftKey - 1 ].m_val, 
					m_keys[ leftKey ].m_time, 
					m_keys[ leftKey ].m_val, 
					m_keys[ rightKey ].m_time, 
					m_keys[ rightKey ].m_val,
					m_keys[ rightKey + 1 ].m_time, 
					m_keys[ rightKey + 1 ].m_val,  
					animTime, interpolated );					
			}
		}

		return interpolated;		
	}
	

	typedef std::vector< CKey > TKeyArray;
	TKeyArray	m_key;	
};

#endif

#endif
