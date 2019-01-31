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
	~Object();

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
