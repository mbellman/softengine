#pragma once

#include <System/Math.h>
#include <Graphics/Color.h>
#include <Graphics/TextureBuffer.h>

/**
 * Vertex2d
 * --------
 */
struct Vertex2d : Colorable {
	Coordinate coordinate;
	int depth;
	float w;
	Vec2 uv;
	Vec3 textureIntensity = { 1.0f, 1.0f, 1.0f };
	Vec3 worldVector;

	static Vertex2d lerp(const Vertex2d& v1, const Vertex2d& v2, float r);
};

/**
 * Vertex3d
 * --------
 */
struct Vertex3d : Colorable {
	Vec3 vector;
	Vec2 uv;

	static Vertex3d lerp(const Vertex3d& v1, const Vertex3d& v2, float r);
};

/**
 * Triangle
 * --------
 */
struct Triangle {
	Vertex2d vertices[3];
	Vec3 normal;
	const TextureBuffer* texture = NULL;

	float averageDepth() const;
};

/**
 * Polygon
 * -------
 */
struct Polygon {
	Vertex3d* vertices[3];
	Vec3 normal;

	void bindVertex(int index, Vertex3d* vertex);
};
