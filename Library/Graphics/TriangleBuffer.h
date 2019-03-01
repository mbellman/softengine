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
	Triangle* requestTriangle();
	void reset();

private:
	constexpr static int TRIANGLE_POOL_SIZE = 100000;

	Level* activeLevel = NULL;
	bool isSwapped = false;
	int totalRequestedTriangles = 0;

	std::vector<Triangle*> triangleBufferA;
	std::vector<Triangle*> triangleBufferB;
	Triangle* trianglePoolA;
	Triangle* trianglePoolB;
};
