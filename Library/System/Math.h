#pragma once

struct RotationMatrix;

/**
 * Coordinate
 * ----------
 */
struct Coordinate {
	int x = 0;
	int y = 0;

	static Coordinate lerp(const Coordinate& c1, const Coordinate& c2, float r);
};

/**
 * Vec2
 * ----
 */
struct Vec2 {
	float x = 0.0f;
	float y = 0.0f;

	static Vec2 lerp(const Vec2& v1, const Vec2& v2, float r);
	Vec2 operator /(float divisor) const;
};

/**
 * Vec3
 * ----
 */
struct Vec3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vec3();
	Vec3(float x, float y, float z);

	static Vec3 crossProduct(const Vec3& v1, const Vec3& v2);
	static float dotProduct(const Vec3& v1, const Vec3& v2);
	static Vec3 lerp(const Vec3& v1, const Vec3& v2, float r);
	float magnitude() const;
	void rotate(const RotationMatrix& rotationMatrix);
	Vec3 unit() const;
	Vec3 operator +(const Vec3& vector) const;
	Vec3 operator -(const Vec3& vector) const;
};

/**
 * RotationMatrix
 * --------------
 */
struct RotationMatrix {
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;

	static RotationMatrix calculate(const Vec3& rotation);
	RotationMatrix operator *(const RotationMatrix& rotationMatrix) const;
	Vec3 operator *(const Vec3& vector) const;
};

/**
 * Range
 * -----
 */
template<class T>
struct Range {
	T start;
	T end;
};
