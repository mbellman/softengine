#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <Types.h>
#include <Loaders/ObjLoader.h>

/**
 * Object
 * ------
 */
struct Object {
	Vec3 position;

	Object();
	virtual ~Object();

	void forEachPolygon(std::function<void(const Polygon&)> handle);
	int getPolygonCount();
	void rotate(const Vec3& rotation);
	void scale(float scalar);
	void setColor(int R, int G, int B);
	void setColor(const Color& color);

protected:
	std::vector<Vertex3d> vertices;

	void addPolygon(int v1, int v2, int v3);
	void addVertex(const Vec3& vector, const Color& color);
	void computeSurfaceNormals();

private:
	std::vector<Polygon> polygons;

	static Vec3 computePolygonNormal(const Polygon& polygon);
};

/**
 * Model
 * -----
 */
struct Model : Object {
	Model(const ObjLoader& obj);
};

/**
 * Mesh
 * ----
 */
struct Mesh : Object {
	Mesh(int rows, int columns, float tileSize);
};

/**
 * Cube
 * ----
 */
struct Cube : Object {
	Cube(float radius);

private:
	static int polygonVertices[12][3];
};

/**
 * Light
 * -----
 */
struct Light : Object {
	Color color = { 255, 255, 255 };
	float power = 1;
	float spread = 500;

	static bool isLight(Object* object);
	void setColor(int R, int G, int B);
	void setColor(const Color& color);
};
