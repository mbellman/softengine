#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <System/Math.h>
#include <System/Geometry.h>
#include <Graphics/Color.h>
#include <Loaders/ObjLoader.h>
#include <Graphics/TextureBuffer.h>

/**
 * Object
 * ------
 */
struct Object {
	Vec3 position;
	TextureBuffer* texture = NULL;

	Object();
	virtual ~Object();

	const std::vector<Polygon>& getPolygons() const;
	int getPolygonCount() const;
	int getVertexCount() const;
	void rotate(const Vec3& rotation);
	void scale(float scalar);
	void setColor(int R, int G, int B);
	void setColor(const Color& color);
	void setTexture(TextureBuffer* textureBuffer);

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

	void setFaceUVCoordinates(float x1, float y1, float x2, float y2);

private:
	static Vec3 vertexPositions[24];
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
	void flip();
	void on();
	void off();
	void setColor(int R, int G, int B);
	void setColor(const Color& color);
};
