#pragma once

#include <System/Math.h>
#include <Graphics/Color.h>
#include <Graphics/TextureBuffer.h>
#include <map>

struct Polygon;
struct Object;

/**
 * Vertex2d
 * --------
 */
struct Vertex2d : Colorable {
	Coordinate coordinate;
	float z;
	float inverseDepth;
	Vec2 perspectiveUV;
	Vec3 textureIntensity = { 1.0f, 1.0f, 1.0f };
	Vec3 worldVector;
	Vec3 normal;

	static Vertex2d lerp(const Vertex2d& v1, const Vertex2d& v2, float r);
};

/**
 * Vertex3d
 * --------
 */
struct Vertex3d : Colorable {
	Vec3 vector;
	Vec3 normal;
	Vec2 uv;
	std::vector<Polygon*> connectedPolygons;
	std::vector<Vec3> morphTargets;

	static Vertex3d lerp(const Vertex3d& v1, const Vertex3d& v2, float r);
	void rotate(const RotationMatrix& rotationMatrix);
	void scale(float scalar);
	void scale(const Vec3& scaleVector);
};

/**
 * Triangle
 * --------
 */
struct Triangle {
	Vertex2d vertices[3];
	Polygon* sourcePolygon = NULL;
	float fresnelFactor = 0.0f;

	/**
	 * Determines whether the triangle is the result of
	 * a polygon clipped against the near plane. Synthetic
	 * triangles are not subject to light caching due to
	 * their ephemeral nature.
	 */
	bool isSynthetic = false;

	float maxZ() const;
};

/**
 * Polygon
 * -------
 */
struct Polygon {
	Vertex3d* vertices[3];
	Vec3 normal;
	Vec3 cachedVertexColorIntensities[3];
	const Object* sourceObject = NULL;

	void bindVertex(int index, Vertex3d* vertex);
};

/**
 * Bounds
 * -----------
 */
struct Bounds {
	Vec3 cornerA;
	Vec3 cornerB;

	bool hasPointInside(const Vec3& point) const;
	bool isColliding(const Bounds& bounds) const;
};

/**
 * Sector
 * ------
 */
struct Sector {
	Bounds bounds;
	int id;
};
