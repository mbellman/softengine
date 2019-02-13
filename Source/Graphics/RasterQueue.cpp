#include <Graphics/RasterQueue.h>
#include <algorithm>
#include <Helpers.h>
#include <System/Geometry.h>

/**
 * RasterQueue
 * -----------
 */
RasterQueue::RasterQueue(int width, int height) {
	rasterWidth = width;
	rasterHeight = height;
}

void RasterQueue::addCover(const Triangle& triangle, int zone) {
	covers.push_back({
		triangle.vertices[0].coordinate,
		triangle.vertices[1].coordinate,
		triangle.vertices[2].coordinate,
		zone,
		isTriangleClockwise(triangle)
	});
}

void RasterQueue::addTriangle(Triangle triangle) {
	int zoneIndex = std::clamp((int)(triangle.averageDepth() / RasterQueue::ZONE_RANGE), 0, RasterQueue::MAX_ZONES - 1);

	if (zoneIndex > highestZoneIndex) {
		highestZoneIndex = zoneIndex;
	}

	if (isTriangleCoverable(triangle)) {
		addCover(triangle, zoneIndex);
	}

	zones[zoneIndex].push_back(triangle);
}

inline bool RasterQueue::isPointInsideEdge(int x, int y, int ex1, int ey1, int ex2, int ey2) {
	return ((x - ex1) * (ey2 - ey1) - (y - ey1) * (ex2 - ex1)) > 0;
}

bool RasterQueue::isTriangleClockwise(const Triangle& triangle) {
	const Coordinate& c0 = triangle.vertices[0].coordinate;
	const Coordinate& c1 = triangle.vertices[1].coordinate;
	const Coordinate& c2 = triangle.vertices[2].coordinate;

	return isPointInsideEdge(c2.x, c2.y, c0.x, c0.y, c1.x, c1.y);
}

bool RasterQueue::isTriangleCoverable(const Triangle& triangle) {
	const Coordinate& c0 = triangle.vertices[0].coordinate;
	const Coordinate& c1 = triangle.vertices[1].coordinate;
	const Coordinate& c2 = triangle.vertices[2].coordinate;

	int minX = std::min(c0.x, std::min(c1.x, c2.x));
	int maxX = std::max(c0.x, std::max(c1.x, c2.x));

	int minY = std::min(c0.y, std::min(c1.y, c2.y));
	int maxY = std::max(c0.y, std::max(c1.y, c2.y));

	return (
		// Ensure that the triangle is over the minimum
		// cover size along both axes
		(maxX - minX) > MIN_COVER_SIZE &&
		(maxY - minY) > MIN_COVER_SIZE &&
		// Ensure that it extends far enough into the screen
		// that distant triangles are likely to be covered by it
		(minX < (rasterWidth - MIN_COVER_SIZE) && maxX > MIN_COVER_SIZE) &&
		(minY < (rasterHeight - MIN_COVER_SIZE) && maxY > MIN_COVER_SIZE)
	);
}

/**
 * Determines whether a Triangle is occluded by a Cover.
 */
bool RasterQueue::isTriangleOccluded(const Triangle& triangle, const Cover& cover) {
	for (int i = 0; i < 3; i++) {
		const Coordinate& tc = triangle.vertices[i].coordinate;

		// To determine whether a triangle T is completely covered by
		// another triangle T*, we have to check T's vertices against
		// T*'s edges. Clockwise/counterclockwise orientation determines
		// the order of the edge vertices we have to compare against.
		// Note that 'clockwise' in the context presented here means
		// clockwise in raster space with its inverted y axis; thus
		// vertex winding order is opposite to the apparent order
		// displayed onscreen.
		if (cover.isClockwise) {
			// Compare against edges T*v0 -> T*v2, T*v2 -> T*v1, T*v1 -> T*v0
			if (
				isPointInsideEdge(tc.x, tc.y, cover.c0.x, cover.c0.y, cover.c2.x, cover.c2.y) ||
				isPointInsideEdge(tc.x, tc.y, cover.c2.x, cover.c2.y, cover.c1.x, cover.c1.y) ||
				isPointInsideEdge(tc.x, tc.y, cover.c1.x, cover.c1.y, cover.c0.x, cover.c0.y)
			) {
				return false;
			}
		} else {
			// Compare against edges T*v1 -> T*v3, T*v3 -> T*v2, T*v2 -> T*v1
			if (
				isPointInsideEdge(tc.x, tc.y, cover.c0.x, cover.c0.y, cover.c1.x, cover.c1.y) ||
				isPointInsideEdge(tc.x, tc.y, cover.c1.x, cover.c1.y, cover.c2.x, cover.c2.y) ||
				isPointInsideEdge(tc.x, tc.y, cover.c2.x, cover.c2.y, cover.c0.x, cover.c0.y)
			) {
				return false;
			}
		}
	}

	return true;
}

bool RasterQueue::isTriangleOnScreen(const Triangle& triangle) {
	const Coordinate& c0 = triangle.vertices[0].coordinate;
	const Coordinate& c1 = triangle.vertices[1].coordinate;
	const Coordinate& c2 = triangle.vertices[2].coordinate;

	int minX = std::min(c0.x, std::min(c1.x, c2.x));
	int maxX = std::max(c0.x, std::max(c1.x, c2.x));

	int minY = std::min(c0.y, std::min(c1.y, c2.y));
	int maxY = std::max(c0.y, std::max(c1.y, c2.y));

	return (minX < rasterWidth) && (maxX > 0) && (minY < rasterHeight) && (maxY > 0);
}

bool RasterQueue::isTriangleVisible(const Triangle& triangle) {
	if (!isTriangleOnScreen(triangle)) {
		return false;
	}

	for (const auto &cover : covers) {
		if (cover.zone < currentZoneIndex && isTriangleOccluded(triangle, cover)) {
			return false;
		}
	}

	return true;
}

Triangle* RasterQueue::next() {
	Zone* currentZone = &zones[currentZoneIndex];
	bool isEndOfZone = currentElementIndex >= currentZone->size() || currentZone->size() == 0;

	if (isEndOfZone) {
		currentZone->clear();

		currentElementIndex = 0;

		if (currentZoneIndex < highestZoneIndex) {
			currentZoneIndex++;

			return next();
		} else {
			reset();

			return NULL;
		}
	} else {
		Triangle* triangle = &currentZone->at(currentElementIndex++);

		return isTriangleVisible(*triangle) ? triangle : next();
	}
}

void RasterQueue::reset() {
	currentZoneIndex = 0;
	highestZoneIndex = 0;

	covers.clear();
}
