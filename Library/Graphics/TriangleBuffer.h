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
	void illuminateTriangle(Triangle* triangle);
	void reset();
	Triangle* requestTriangle();
	void setActiveLevel(Level* activeLevel);

private:
	constexpr static int AMBIENT_LIGHT_ID = 0;
	constexpr static int TRIANGLE_POOL_SIZE = 100000;

	Level* activeLevel = NULL;
	bool isSwapped = false;
	int totalRequestedTriangles = 0;

	std::vector<Triangle*> triangleBufferA;
	std::vector<Triangle*> triangleBufferB;
	Triangle* trianglePoolA;
	Triangle* trianglePoolB;

	Vec3 getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex);
	void illuminateColorTriangle(Triangle* triangle);
	void illuminateTextureTriangle(Triangle* triangle);
};
