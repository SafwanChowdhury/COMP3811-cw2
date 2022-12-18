#ifndef VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD
#define VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD

#include <cmath>
#include <cassert>
#include <cstdlib>

struct Vec3f
{
	float x, y, z;

	constexpr
		float& operator[] (std::size_t aI) noexcept
	{
		assert(aI < 3);
		return aI[&x]; // This is a bit sketchy, but concise and efficient.
	}
	constexpr
		float operator[] (std::size_t aI) const noexcept
	{
		assert(aI < 3);
		return aI[&x]; // This is a bit sketchy.
	}
};


constexpr
Vec3f operator+(Vec3f aVec) noexcept
{
	return aVec;
}
constexpr
Vec3f operator-(Vec3f aVec) noexcept
{
	return { -aVec.x, -aVec.y, -aVec.z };
}

constexpr
Vec3f operator+(Vec3f aLeft, Vec3f aRight) noexcept
{
	return Vec3f{
		aLeft.x + aRight.x,
		aLeft.y + aRight.y,
		aLeft.z + aRight.z
	};
}
constexpr
Vec3f operator-(Vec3f aLeft, Vec3f aRight) noexcept
{
	return Vec3f{
		aLeft.x - aRight.x,
		aLeft.y - aRight.y,
		aLeft.z - aRight.z
	};
}


constexpr
Vec3f operator*(float aScalar, Vec3f aVec) noexcept
{
	return Vec3f{
		aScalar * aVec.x,
		aScalar * aVec.y,
		aScalar * aVec.z
	};
}
constexpr
Vec3f operator*(Vec3f aVec, float aScalar) noexcept
{
	return aScalar * aVec;
}

constexpr
Vec3f operator/(Vec3f aVec, float aScalar) noexcept
{
	return Vec3f{
		aVec.x / aScalar,
		aVec.y / aScalar,
		aVec.z / aScalar
	};
}


constexpr
Vec3f& operator+=(Vec3f& aLeft, Vec3f aRight) noexcept
{
	aLeft.x += aRight.x;
	aLeft.y += aRight.y;
	aLeft.z += aRight.z;
	return aLeft;
}
constexpr
Vec3f& operator-=(Vec3f& aLeft, Vec3f aRight) noexcept
{
	aLeft.x -= aRight.x;
	aLeft.y -= aRight.y;
	aLeft.z -= aRight.z;
	return aLeft;
}

constexpr
Vec3f& operator*=(Vec3f& aLeft, float aRight) noexcept
{
	aLeft.x *= aRight;
	aLeft.y *= aRight;
	aLeft.z *= aRight;
	return aLeft;
}
constexpr
Vec3f& operator/=(Vec3f& aLeft, float aRight) noexcept
{
	aLeft.x /= aRight;
	aLeft.y /= aRight;
	aLeft.z /= aRight;
	return aLeft;
}


// A few common functions:

constexpr
float dot(Vec3f aLeft, Vec3f aRight) noexcept
{
	return aLeft.x * aRight.x
		+ aLeft.y * aRight.y
		+ aLeft.z * aRight.z
		;
}

inline
float length(Vec3f aVec) noexcept
{
	// The standard function std::sqrt() is not marked as constexpr. length()
	// calls std::sqrt() unconditionally, so length() cannot be marked
	// constexpr itself.
	return std::sqrt(dot(aVec, aVec));
}

inline
Vec3f normalize(Vec3f aVec) noexcept
{
	auto const l = length(aVec);
	return aVec / l;
}


inline
Vec3f cross(Vec3f aLeft, Vec3f aRight) noexcept
{
	Vec3f newVec{};
	newVec.x = aLeft.y * aRight.z - aLeft.z * aRight.y;
	newVec.y = -(aLeft.x * aRight.z - aLeft.z * aRight.x);
	newVec.z = aLeft.x * aRight.y - aLeft.y * aRight.x;
	return newVec;
}

inline
Vec3f reflect(Vec3f aIncidence, Vec3f aNormal) {
	Vec3f newVec{};
	newVec = aIncidence - 2.f * 
		(aIncidence.x * aNormal.x
		+ aIncidence.y * aNormal.y
		+ aIncidence.z * aNormal.z)
		* aNormal;
	return newVec;
}

#endif // VEC3_HPP_5710DADF_17EF_453C_A9C8_4A73DC66B1CD
