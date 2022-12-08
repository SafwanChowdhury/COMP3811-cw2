#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f newMatrix{};
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 4; ++k)
			{
				newMatrix(i, j) += aLeft(i, k) * aRight(k, j);
			}

	return Mat44f{ newMatrix };
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f newVector{};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; j++)
		{
			newVector[i] += (aRight[j] * aLeft(i, j));
		}
	}
	return Vec4f{ newVector };
}

// Functions:

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f rMatrix{};
	rMatrix(0, 0) = 1;
	rMatrix(3, 3) = 1;
	rMatrix(1, 1) = cos(aAngle); //use matrix rotation rules to rotate matrix by angle
	rMatrix(1, 2) = -sin(aAngle);
	rMatrix(2, 1) = sin(aAngle);
	rMatrix(2, 2) = cos(aAngle);
	return Mat44f{ rMatrix };
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f rMatrix{};
	rMatrix(1, 1) = 1;
	rMatrix(3, 3) = 1;
	rMatrix(0, 0) = cos(aAngle); //use matrix rotation rules to rotate matrix by angle
	rMatrix(0, 2) = sin(aAngle);
	rMatrix(2, 0) = -sin(aAngle);
	rMatrix(2, 2) = cos(aAngle);
	return Mat44f{ rMatrix };
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f rMatrix{};
	rMatrix(2, 2) = 1;
	rMatrix(3, 3) = 1;
	rMatrix(0, 0) = cos(aAngle); //use matrix rotation rules to rotate matrix by angle
	rMatrix(0, 1) = -sin(aAngle);
	rMatrix(1, 0) = -sin(aAngle);
	rMatrix(1, 1) = cos(aAngle);
	return Mat44f{ rMatrix };
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f rMatrix{};
	rMatrix(0, 0) = 1;
	rMatrix(1, 1) = 1;
	rMatrix(2, 2) = 1;
	rMatrix(3, 3) = 1;
	rMatrix(0, 3) = aTranslation[0]; //use matrix translation rules to translate matrix by vector
	rMatrix(1, 3) = aTranslation[1];
	rMatrix(2, 3) = aTranslation[2];
	return Mat44f{ rMatrix };
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	Mat44f rMatrix{};
	rMatrix(0, 0) = aSX;
	rMatrix(1, 1) = aSY;
	rMatrix(2, 2) = aSZ;
	rMatrix(3, 3) = 1;
	return Mat44f{ rMatrix };
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float s = 1 / (tan(aFovInRadians / 2));
	float sx = s / aAspect;
	float sy = s;
	float a = -((aFar + aNear) / (aFar - aNear));
	float b = -2 * ((aFar * aNear) / (aFar - aNear));
	Mat44f rMatrix{};
	rMatrix(0, 0) = sx;
	rMatrix(1, 1) = sy;
	rMatrix(2, 2) = a;
	rMatrix(2, 3) = b;
	rMatrix(3, 2) = -1;
	rMatrix(3, 3) = 1;
	return Mat44f{ rMatrix };
}



#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
