#pragma once

#include <vector>
#include <Types.h>

/**
 * Zone
 * ----
 *
 * A lineup of Triangle objects corresponding to a given zone.
 */
typedef std::vector<Triangle> Zone;

/**
 * Cover
 * -----
 *
 * Represents the screen coverage of larger Triangles which
 * are capable of occluding others behind them.
 */
struct Cover {
	Coordinate c1;
	Coordinate c2;
	Coordinate c3;
	int zone;
};

/**
 * RasterQueue
 * -----------
 *
 * Provides a mechanism for receiving and storing Triangles by 'zone'
 * before they are drawn, and dispensing them in zone-order, also
 * filtering out triangles occluded by larger, closer ones.
 */
class RasterQueue {
public:
	RasterQueue(int width, int height);

	void addTriangle(Triangle triangle, int zoneIndex);
	Triangle* next();

private:
	constexpr static int MAX_ZONES = 50;
	constexpr static int MIN_COVER_SIZE = 150;
	int currentZoneIndex = 0;
	int highestZoneIndex = 0;
	int currentElementIndex = 0;
	int rasterWidth;
	int rasterHeight;
	Zone zones[RasterQueue::MAX_ZONES];
	std::vector<Cover> covers;

	void addCover(const Triangle& triangle, int zone);
	bool isPointWithinEdge(int x, int y, int ex1, int ey1, int ex2, int ey2);
	bool isTriangleCoverable(const Triangle& triangle);
	bool isTriangleNearby(const Triangle& triangle, const Cover& cover);
	bool isTriangleOccluded(const Triangle& triangle, const Cover& cover);
	bool isTriangleVisible(const Triangle& triangle);
	void reset();
};
