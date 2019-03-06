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

	vertex.color = Color::lerp(v1.color, v2.color, r);
	vertex.coordinate.x = Lerp::lerp(v1.coordinate.x, v2.coordinate.x, r);
	vertex.coordinate.y = Lerp::lerp(v1.coordinate.y, v2.coordinate.y, r);
	vertex.z = Lerp::lerp(v1.z, v2.z, r);
	vertex.inverseDepth = Lerp::lerp(v1.inverseDepth, v2.inverseDepth, r);
	vertex.perspectiveUV = Vec2::lerp(v1.perspectiveUV, v2.perspectiveUV, r);
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
	vertex.normal = ((v1.normal + v2.normal) / 2.0f).unit();

	return vertex;
}

void Vertex3d::rotate(const RotationMatrix& rotationMatrix) {
	vector.rotate(rotationMatrix);

	for (auto& morphTarget : morphTargets) {
		morphTarget.rotate(rotationMatrix);
	}
}

void Vertex3d::scale(float scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	vector.z *= scalar;

	for (auto& morphTarget : morphTargets) {
		morphTarget.x *= scalar;
		morphTarget.y *= scalar;
		morphTarget.z *= scalar;
	}
}

void Vertex3d::scale(const Vec3& scaleVector) {
	vector.x *= scaleVector.x;
	vector.y *= scaleVector.y;
	vector.z *= scaleVector.z;

	for (auto& morphTarget : morphTargets) {
		morphTarget.x *= scaleVector.x;
		morphTarget.y *= scaleVector.y;
		morphTarget.z *= scaleVector.z;
	}
}

/**
 * Triangle
 * --------
 */
float Triangle::maxZ() const {
	return FAST_MAX(vertices[0].z, FAST_MAX(vertices[1].z, vertices[2].z));
}

/**
 * Polygon
 * -------
 */
void Polygon::bindVertex(int index, Vertex3d* vertex) {
	vertices[index] = vertex;

	vertex->connectedPolygons.push_back(this);
}

/**
 * Bounds
 * ------
 */
bool Bounds::hasPointInside(const Vec3& point) const {
	return (
		point.x > cornerA.x && point.x < cornerB.x &&
		point.y > cornerA.y && point.y < cornerB.y &&
		point.z > cornerA.z && point.z < cornerB.z
	);
}

bool Bounds::isColliding(const Bounds& bounds) const {
	return !(
		cornerB.x < bounds.cornerA.x || cornerA.x > bounds.cornerB.x ||
		cornerB.y < bounds.cornerA.y || cornerA.y > bounds.cornerB.y ||
		cornerB.z < bounds.cornerA.z || cornerA.z > bounds.cornerB.z
	);
}
