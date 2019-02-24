#pragma once

#include <System/Geometry.h>
#include <System/Level.h>
#include <vector>

class TriangleBuffer {
public:
	TriangleBuffer();
	~TriangleBuffer();

	void bufferTriangle(Triangle* triangle);
	const std::vector<Triangle*>& getBufferedTriangles();
	int getTotalRequestedTriangles();
	int getTotalNonStaticTriangles();
	void illuminateTriangle(Triangle* triangle);
	void illuminateStaticPolygon(Polygon* polygon);
	void reset();
	Triangle* requestTriangle();
	void setActiveLevel(Level* activeLevel);

private:
	constexpr static int TRIANGLE_POOL_SIZE = 100000;

	Level* activeLevel = NULL;
	bool isSwapped = false;
	int totalRequestedTriangles = 0;

	std::vector<Triangle*> triangleBufferA;
	std::vector<Triangle*> triangleBufferB;
	Triangle* trianglePoolA;
	Triangle* trianglePoolB;

	void computeAmbientLightColorIntensity(const Vec3& normal, Vec3& colorIntensity);
	void computeLightColorIntensity(const Light* light, const Vec3& vertexPosition, const Vec3& normal, Vec3& colorIntensity);
	Vec3 getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex);
	void illuminateColorTriangle(Triangle* triangle);
	void illuminateTextureTriangle(Triangle* triangle);
};
