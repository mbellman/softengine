#include <System/Geometry.h>
#include <System/Math.h>
#include <Graphics/Color.h>
#include <Helpers.h>

/**
 * Vertex2d
 * --------
 */
Vertex2d Vertex2d::lerp(const Vertex2d& v1, const Vertex2d& v2, float r) {
	Vertex2d vertex;

	vertex.coordinate.x = Lerp::lerp(v1.coordinate.x, v2.coordinate.x, r);
	vertex.coordinate.y = Lerp::lerp(v1.coordinate.y, v2.coordinate.y, r);
	vertex.color = Color::lerp(v1.color, v2.color, r);
	vertex.depth = Lerp::lerp(v1.depth, v2.depth, r);
	vertex.uv = Vec2::lerp(v1.uv, v2.uv, r);
	vertex.w = Lerp::lerp(v1.w, v2.w, r);
	vertex.textureIntensity = Vec3::lerp(v1.textureIntensity, v2.textureIntensity, r);

	return vertex;
}

/**
 * Vertex3d
 * --------
 */
Vertex3d Vertex3d::lerp(const Vertex3d& v1, const Vertex3d& v2, float r) {
	Vertex3d vertex;

	vertex.vector = Vec3::lerp(v1.vector, v2.vector, r);
	vertex.uv = Vec2::lerp(v1.uv, v2.uv, r);
	vertex.color = Color::lerp(v1.color, v2.color, r);

	return vertex;
}

/**
 * Triangle
 * --------
 */
float Triangle::averageDepth() const {
	return (vertices[0].depth + vertices[1].depth + vertices[2].depth) / 3;
}

/**
 * Polygon
 * -------
 */
void Polygon::bindVertex(int index, Vertex3d* vertex) {
	vertices[index] = vertex;
}
