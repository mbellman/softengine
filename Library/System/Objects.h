#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <System/Math.h>
#include <System/Geometry.h>
#include <System/Positionable.h>
#include <Graphics/Color.h>
#include <Loaders/ObjLoader.h>
#include <Graphics/TextureBuffer.h>

/**
 * Object
 * ------
 */
struct Object : public Positionable {
	bool isStatic = false;
	bool isFlatShaded = false;
	Vec3 velocity;
	float fresnelFactor = 0.0f;
	TextureBuffer* texture = NULL;

	Object();
	virtual ~Object();

	void computeSurfaceNormals();
	int getId() const;
	const std::vector<Polygon*>& getPolygons() const;
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

	void addPolygon(int v1_index, int v2_index, int v3_index);
	void addVertex(const Vec3& vector);
	void addVertex(const Vec3& vector, const Color& color);
	void addVertex(const Vec3& vector, const Vec2& color);

private:
	std::vector<Polygon*> polygons;
	int id;

	static Vec3 computePolygonNormal(const Polygon& polygon);
	static Vec3 computeVertexNormal(const Vertex3d& vertex);
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
	void setVertexOffsets(std::function<void(int, int, Vec3&)> offsetHandler);

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

/**
 * DirectionalLight
 * ----------------
 */
struct DirectionalLight : Light {
	const Vec3& getDirection() const;
	void setDirection(const Vec3& direction);
private:
	Vec3 direction = { 0, -1, 0 };
};
