#pragma once

#include <memory>

struct Color {
	int R = 255;
	int G = 255;
	int B = 255;
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
	float magnitude();
	Vec3 unit();
	Vec3 rotate(const Vec3& rotation);
	Vec3 operator +(const Vec3& vector) const;
};

struct RotationMatrix {
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;

	static RotationMatrix calculate(const Vec3& rotation) {
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

	RotationMatrix operator *(const RotationMatrix& rm) const;
	Vec3 operator *(const Vec3& v) const;
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
	void createVertex(int index, const Coordinate& coordinate, int depth, const Color& color);
};

struct Polygon {
	Vertex3d* vertices[3];
	void bindVertex(int index, Vertex3d* vertex);
};

