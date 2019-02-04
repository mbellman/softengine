#include <memory>
#include <cmath>
#include <algorithm>
#include <Types.h>

using namespace std;

/**
 * Color
 * -----
 */
int Color::normalize(int component) {
	return clamp(component, 0, 255);
}

Color Color::operator +(int illumination) const {
	return {
		normalize(R + illumination),
		normalize(G + illumination),
		normalize(B + illumination)
	};
}

Color Color::operator +(const Color& color) const {
	return {
		normalize(R + color.R),
		normalize(G + color.G),
		normalize(B + color.B)
	};
}

Color Color::operator +=(const Color& color) {
	R = normalize(R + color.R);
	G = normalize(G + color.G);
	B = normalize(B + color.B);

	return *this;
}

Color Color::operator -(int attenuation) const {
	return {
		normalize(R - attenuation),
		normalize(G - attenuation),
		normalize(B - attenuation)
	};
}

Color Color::operator -(const Color& color) const {
	return {
		normalize(R - color.R),
		normalize(G - color.G),
		normalize(B - color.B)
	};
}

Color Color::operator -=(const Color& color) {
	R = normalize(R + color.R);
	G = normalize(G + color.G);
	B = normalize(B + color.B);

	return *this;
}

Color Color::operator *(float multiplier) const {
	return {
		normalize((int)(R * multiplier)),
		normalize((int)(G * multiplier)),
		normalize((int)(B * multiplier))
	};
}

Color Color::operator *=(float multiplier) {
	R = normalize((int)(R * multiplier));
	G = normalize((int)(G * multiplier));
	B = normalize((int)(B * multiplier));

	return *this;
}

/**
 * RotationMatrix
 * --------------
 */
RotationMatrix RotationMatrix::operator *(const RotationMatrix& rm) const {
	return {
		m11 * rm.m11 + m12 * rm.m21 + m13 * rm.m31, m11 * rm.m12 + m12 * rm.m22 + m13 * rm.m32, m11 * rm.m13 + m12 * rm.m23 + m13 * rm.m33,
		m21 * rm.m11 + m22 * rm.m21 + m23 * rm.m31, m21 * rm.m12 + m22 * rm.m22 + m23 * rm.m32, m21 * rm.m13 + m22 * rm.m23 + m23 * rm.m33,
		m31 * rm.m11 + m32 * rm.m21 + m33 * rm.m31, m31 * rm.m12 + m32 * rm.m22 + m33 * rm.m32, m31 * rm.m13 + m32 * rm.m23 + m33 * rm.m33
	};
}

RotationMatrix RotationMatrix::calculate(const Vec3& rotation) {
	float sx = sin(rotation.x);
	float sy = sin(rotation.y);
	float sz = sin(rotation.z);
	float cx = cos(rotation.x);
	float cy = cos(rotation.y);
	float cz = cos(rotation.z);

	RotationMatrix rX = { 1, 0, 0, 0, cx, -sx, 0, sx, cx };
	RotationMatrix rY = { cy, 0, sy, 0, 1, 0, -sy, 0, cy };
	RotationMatrix rZ = { cz, -sz, 0, sz, cz, 0, 0, 0, 1 };

	return rZ * rY * rX;
}

Vec3 RotationMatrix::operator *(const Vec3& v) const {
	return {
		m11 * v.x + m12 * v.y + m13 * v.z,
		m21 * v.x + m22 * v.y + m23 * v.z,
		m31 * v.x + m32 * v.y + m33 * v.z
	};
}

/**
 * Vec3
 * ----
 */
Vec3::Vec3() {}

Vec3::Vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3 Vec3::crossProduct(const Vec3& v1, const Vec3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

float Vec3::dotProduct(const Vec3& v1, const Vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vec3::magnitude() {
	return sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::unit() {
	float m = magnitude();

	return {
		x / m,
		y / m,
		z / m
	};
}

void Vec3::rotate(const RotationMatrix& rotationMatrix) {
	Vec3 rotatedVector = rotationMatrix * (*this);

	x = rotatedVector.x;
	y = rotatedVector.y;
	z = rotatedVector.z;
}

Vec3 Vec3::operator +(const Vec3& vector) const {
	return {
		x + vector.x,
		y + vector.y,
		z + vector.z
	};
}

Vec3 Vec3::operator -(const Vec3& vector) const {
	return {
		x - vector.x,
		y - vector.y,
		z - vector.z
	};
}

/**
 * Triangle
 * --------
 */
float Triangle::averageDepth() const {
	return (vertices[0].depth + vertices[1].depth + vertices[2].depth) / 3;
}

void Triangle::createVertex(int index, int x, int y, int depth, const Color& color) {
	Vertex2d vertex;

	vertex.coordinate.x = x;
	vertex.coordinate.y = y;
	vertex.depth = depth;
	vertex.color = color;

	vertices[index] = vertex;
}

/**
 * Polygon
 * -------
 */
void Polygon::bindVertex(int index, Vertex3d* vertex) {
	vertices[index] = vertex;
}
