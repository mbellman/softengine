#pragma once

#include <vector>
#include <optional>
#include <Types.h>

typedef std::vector<Triangle> Zone;

struct Cover {
	Coordinate c1;
	Coordinate c2;
	Coordinate c3;
	int zone;
};

class RasterQueue {
public:
	RasterQueue(int width, int height);

	void addTriangle(Triangle triangle, int zoneIndex);
	int DEBUG_getTotalCovers();
	Triangle* next();

private:
	constexpr static int MAX_ZONES = 50;
	constexpr static int MIN_COVER_SIZE = 150;
	int currentZoneIndex = 0;
	int highestZoneIndex = 0;
	int currentElementIndex = 0;
	int width;
	int height;
	Zone zones[RasterQueue::MAX_ZONES];
	std::vector<Cover> covers;

	void addCover(Triangle& triangle, int zone);
	bool isTriangleCoverable(Triangle& triangle);
	bool isTriangleVisible(Triangle& triangle);
	bool isTriangleOccluded(Triangle& triangle, const Cover& cover);
	bool isPointWithinEdge(int x, int y, int ex1, int ey1, int ex2, int ey2);
	void reset();
};
