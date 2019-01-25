#include <memory>
#include <Types.h>

RotationMatrix RotationMatrix::operator *(const RotationMatrix& rm) const {
	return {
		m11 * rm.m11 + m12 * rm.m21 + m13 * rm.m31, m11 * rm.m12 + m12 * rm.m22 + m13 * rm.m32, m11 * rm.m13 + m12 * rm.m23 + m13 * rm.m33,
		m21 * rm.m11 + m22 * rm.m21 + m23 * rm.m31, m21 * rm.m12 + m22 * rm.m22 + m23 * rm.m32, m21 * rm.m13 + m22 * rm.m23 + m23 * rm.m33,
		m31 * rm.m11 + m32 * rm.m21 + m33 * rm.m31, m31 * rm.m12 + m32 * rm.m22 + m33 * rm.m32, m31 * rm.m13 + m32 * rm.m23 + m33 * rm.m33
	};
}

Vec3 RotationMatrix::operator *(const Vec3& v) const {
	return {
		m11 * v.x + m12 * v.y + m13 * v.z,
		m21 * v.x + m22 * v.y + m23 * v.z,
		m31 * v.x + m32 * v.y + m33 * v.z
	};
}

Vec3::Vec3() {}

Vec3::Vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vec3::magnitude() {
	return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::unit() {
	float m = magnitude();

	return {
		x / m,
		y / m,
		z / m
	};
}

Vec3 Vec3::rotate(const Vec3& rotation) {
	return RotationMatrix::calculate(rotation) * (*this);
}

Vec3 Vec3::operator +(const Vec3& vector) const {
	return {
		x + vector.x,
		y + vector.y,
		z + vector.z
	};
}

void Triangle::createVertex(int index, const Coordinate& coordinate, int depth, const Color& color) {
	Vertex2d vertex;

	vertex.coordinate = coordinate;
	vertex.depth = depth;
	vertex.color = color;

	vertices[index] = vertex;
}

void Polygon::bindVertex(int index, Vertex3d* vertex) {
	vertices[index] = vertex;
}
