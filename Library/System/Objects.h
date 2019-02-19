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
	bool isStatic = false;
	Vec3 position;
	Vec3 velocity;
	TextureBuffer* texture = NULL;

	Object();
	virtual ~Object();

	int getId() const;
	const std::vector<Polygon>& getPolygons() const;
	int getPolygonCount() const;
	int getVertexCount() const;

	template<class T>
	bool isOfType() {
		return dynamic_cast<T*>(this) != NULL;
	}

	void rotate(const Vec3& rotation);
	void rotateDeg(const Vec3& rotation);
	void scale(float scalar);
	void scale(const Vec3& vector);
	void setColor(int R, int G, int B);
	void setColor(const Color& color);
	void setTexture(TextureBuffer* textureBuffer);
	void update(int dt);

protected:
	std::vector<Vertex3d> vertices;

	void addPolygon(int v1, int v2, int v3);
	void addVertex(const Vec3& vector);
	void addVertex(const Vec3& vector, const Color& color);
	void addVertex(const Vec3& vector, const Vec2& color);
	void computeSurfaceNormals();

private:
	std::vector<Polygon> polygons;
	int id;

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

	void setTextureInterval(int rowInterval, int columnInterval);

private:
	int rows = 0;
	int columns = 0;
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
 * Particle
 * --------
 */
struct Particle : Object {
	bool shouldReset = true;

	Particle();
};

/**
 * Light
 * -----
 */
struct Light : Object {
	float power = 1.0f;
	float range = 500;
	bool isDisabled = false;

	const Color& getColor() const;
	const Vec3& getColorRatios() const;
	void setColor(int R, int G, int B);
	void setColor(const Color& color);

private:
	Color color = { 255, 255, 255 };
	Vec3 cachedColorRatios = { 1.0f, 1.0f, 1.0f };
};
