#pragma once

#include <vector>
#include <System/Geometry.h>

/**
 * Cover
 * -----
 *
 * Represents the screen coverage of larger Triangles which
 * are capable of occluding others behind them.
 */
struct Cover {
	Coordinate c0;
	Coordinate c1;
	Coordinate c2;
	int zone;
	bool isClockwise;
};

/**
 * RasterFilter
 * ------------
 *
 * Provides a mechanism for receiving and storing Triangles by 'zone'
 * before they are drawn, and dispensing them in zone-order, also
 * filtering out triangles occluded by larger, closer ones.
 */
class RasterFilter {
public:
	RasterFilter(int width, int height);

	void addTriangle(Triangle* triangle);
	Triangle* next();

private:
	/**
	 * Zone
	 * ----
	 *
	 * A lineup of Triangle objects corresponding to a given zone.
	 */
	typedef std::vector<Triangle*> Zone;

	constexpr static int MAX_ZONES = 50;
	constexpr static int MIN_COVER_SIZE = 150;
	constexpr static int ZONE_RANGE = 250;

	int currentZoneIndex = 0;
	int highestZoneIndex = 0;
	int currentElementIndex = 0;
	int rasterWidth;
	int rasterHeight;
	Zone zones[RasterFilter::MAX_ZONES];
	std::vector<Cover> covers;

	void addCover(const Triangle* triangle, int zone);
	inline bool isPointInsideEdge(int x, int y, int ex1, int ey1, int ex2, int ey2);
	bool isTriangleClockwise(const Triangle* triangle);
	bool isTriangleCoverable(const Triangle* triangle);
	bool isTriangleNearby(const Triangle* triangle, const Cover& cover);
	bool isTriangleOccluded(const Triangle* triangle, const Cover& cover);
	bool isTriangleOnScreen(const Triangle* triangle);
	bool isTriangleVisible(const Triangle* triangle);
	void reset();
};
