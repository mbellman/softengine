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

	Vec3() {}

	Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3 operator +(const Vec3& v) const {
		Vec3 vt;

		vt.x = x + v.x;
		vt.y = y + v.y;
		vt.z = z + v.z;

		return vt;
	}
};

struct Vertex2d : Colorable {
	Coordinate coordinate;
};

struct Vertex3d : Colorable {
	Vec3 vector;
};

struct Triangle {
	Vertex2d vertices[3];

	void setVertex(int index, const Coordinate& coordinate, const Color& color) {
		Vertex2d vertex;

		vertex.coordinate = coordinate;
		vertex.color = color;

		vertices[index] = vertex;
	}
};

struct Polygon {
	Vertex3d* vertices[3];

	void followVertex(int index, Vertex3d* vertex) {
		vertices[index] = vertex;
	}
};

