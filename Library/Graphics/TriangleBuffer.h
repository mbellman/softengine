#pragma once

#include <System/Geometry.h>
#include <System/Level.h>
#include <vector>

class TriangleBuffer {
public:
	TriangleBuffer();
	~TriangleBuffer();

	void bufferTriangle(Triangle* triangle);
	void clear();
	const std::vector<Triangle*>& getBufferedTriangles();
	int getTotalRequestedTriangles();
	void illuminateTriangle(Triangle* triangle);
	Triangle* requestTriangle();
	void setActiveLevel(Level* activeLevel);

private:
	constexpr static int AMBIENT_LIGHT_ID = 0;
	constexpr static int TRIANGLE_POOL_SIZE = 200000;

	int totalRequestedTriangles = 0;
	Triangle* trianglePool;
	std::vector<Triangle*> bufferedTriangles;

	Level* activeLevel = NULL;

	Vec3 getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex);
	void illuminateColorTriangle(Triangle* triangle);
	void illuminateTextureTriangle(Triangle* triangle);
};
