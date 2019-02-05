#pragma once

struct RotationMatrix;
struct Polygon;
struct Object;

/**
 * Color
 * -----
 */
struct Color {
	int R = 255;
	int G = 255;
	int B = 255;

	Color operator +(int attenuation) const;
	Color operator +(const Color& color) const;
	Color operator +=(const Color& color);
	Color operator -(int attenuation) const;
	Color operator -(const Color& color) const;
	Color operator -=(const Color& color);
	Color operator *(float multiplier) const;
	Color operator *=(float multiplier);

private:
	static int normalize(int component);
};

/**
 * Colorable
 * ---------
 */
struct Colorable {
	Color color;
};

/**
 * Coordinate
 * ----------
 */
struct Coordinate {
	int x = 0;
	int y = 0;
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
 * Vertex2d
 * --------
 */
struct Vertex2d : Colorable {
	Coordinate coordinate;
	int depth;
};

/**
 * Vertex3d
 * --------
 */
struct Vertex3d : Colorable {
	Vec3 vector;
};

/**
 * Triangle
 * --------
 */
struct Triangle {
	Vertex2d vertices[3];
	const Polygon* polygon = NULL;

	float averageDepth() const;
	void createVertex(int index, int x, int y, int depth, const Color& color);
};

/**
 * Polygon
 * -------
 */
struct Polygon {
	Vertex3d* vertices[3];
	Vec3 normal;
	const Object* object = NULL;

	void bindVertex(int index, Vertex3d* vertex);
};
