#pragma once

#include <memory>
#include <algorithm>

struct RotationMatrix;

struct Color {
	int R = 255;
	int G = 255;
	int B = 255;

	Color operator +(int attenuation) const {
		return {
			std::min(R + attenuation, 255),
			std::min(G + attenuation, 255),
			std::min(B + attenuation, 255)
		};
	}

	Color operator -(int attenuation) const {
		return {
			std::max(R - attenuation, 0),
			std::max(G - attenuation, 0),
			std::max(B - attenuation, 0)
		};
	}
};

struct Colorable {
	Color color;
};

struct Coordinate {
	int x = 0;
	int y = 0;
};

struct Vec3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vec3();
	Vec3(float x, float y, float z);

	static Vec3 crossProduct(const Vec3& v1, const Vec3& v2);
	static float dotProduct(const Vec3& v1, const Vec3& v2);
	float magnitude();
	void rotate(const RotationMatrix& rotationMatrix);
	Vec3 unit();
	Vec3 operator +(const Vec3& vector) const;
	Vec3 operator -(const Vec3& vector) const;
};

struct RotationMatrix {
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;

	static RotationMatrix calculate(const Vec3& rotation);
	RotationMatrix operator *(const RotationMatrix& rotationMatrix) const;
	Vec3 operator *(const Vec3& vector) const;
};

struct Vertex2d : Colorable {
	Coordinate coordinate;
	int depth;
};

struct Vertex3d : Colorable {
	Vec3 vector;
};

struct Triangle {
	Vertex2d vertices[3];

	void createVertex(int index, int x, int y, int depth, const Color& color);
};

struct Polygon {
	Vertex3d* vertices[3];
	Vec3 normal;

	void bindVertex(int index, Vertex3d* vertex);
};

