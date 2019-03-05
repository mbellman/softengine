#include <Graphics/RasterFilter.h>
#include <algorithm>
#include <Helpers.h>
#include <System/Geometry.h>
#include <System/Objects.h>

/**
 * RasterFilter
 * ------------
 */
RasterFilter::RasterFilter(int width, int height) {
	rasterWidth = width;
	rasterHeight = height;
}

void RasterFilter::addCover(const Triangle* triangle, int zone) {
	covers.push_back({
		triangle->vertices[0].coordinate,
		triangle->vertices[1].coordinate,
		triangle->vertices[2].coordinate,
		zone,
		isTriangleClockwise(triangle)
	});
}

void RasterFilter::addTriangle(Triangle* triangle) {
	int targetZoneIndex = (int)(triangle->maxZ() / RASTER_FILTER_ZONE_RANGE);
	int maxZoneIndex = MAX_RASTER_FILTER_ZONES - 1;
	int zoneIndex = FAST_CLAMP(targetZoneIndex, 0, maxZoneIndex);

	if (zoneIndex > highestZoneIndex) {
		highestZoneIndex = zoneIndex;
	}

	if (isTriangleCoverable(triangle)) {
		addCover(triangle, zoneIndex);
	}

	zones[zoneIndex].push_back(triangle);
}

inline bool RasterFilter::isPointInsideEdge(int x, int y, int ex1, int ey1, int ex2, int ey2) {
	return ((x - ex1) * (ey2 - ey1) - (y - ey1) * (ex2 - ex1)) >= 0;
}

bool RasterFilter::isTriangleClockwise(const Triangle* triangle) {
	const Coordinate& c0 = triangle->vertices[0].coordinate;
	const Coordinate& c1 = triangle->vertices[1].coordinate;
	const Coordinate& c2 = triangle->vertices[2].coordinate;

	return isPointInsideEdge(c2.x, c2.y, c0.x, c0.y, c1.x, c1.y);
}

bool RasterFilter::isTriangleCoverable(const Triangle* triangle) {
	if (!triangle->sourcePolygon->sourceObject->canOccludeSurfaces) {
		return false;
	}

	const Coordinate& c0 = triangle->vertices[0].coordinate;
	const Coordinate& c1 = triangle->vertices[1].coordinate;
	const Coordinate& c2 = triangle->vertices[2].coordinate;

	int minX = FAST_MIN(c0.x, FAST_MIN(c1.x, c2.x));
	int maxX = FAST_MAX(c0.x, FAST_MAX(c1.x, c2.x));

	if ((maxX - minX) < MIN_COVER_TRIANGLE_SIZE) {
		// Optimize for triangles too horizontally small
		return false;
	}

	int minY = FAST_MIN(c0.y, FAST_MIN(c1.y, c2.y));
	int maxY = FAST_MAX(c0.y, FAST_MAX(c1.y, c2.y));

	if ((maxY - minY) < MIN_COVER_TRIANGLE_SIZE) {
		// Optimize for triangles too vertically small
		return false;
	}

	return (
		// Ensure that the triangle extends far enough into the screen
		// that distant triangles are likely to be covered by it
		(minX < (rasterWidth - MIN_COVER_TRIANGLE_SIZE) && maxX > MIN_COVER_TRIANGLE_SIZE) &&
		(minY < (rasterHeight - MIN_COVER_TRIANGLE_SIZE) && maxY > MIN_COVER_TRIANGLE_SIZE)
	);
}

/**
 * Determines whether a Triangle is occluded by a Cover.
 */
bool RasterFilter::isTriangleOccluded(const Triangle* triangle, const Cover& cover) {
	for (int i = 0; i < 3; i++) {
		const Coordinate& tc = triangle->vertices[i].coordinate;

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

bool RasterFilter::isTriangleOnScreen(const Triangle* triangle) {
	const Coordinate& c0 = triangle->vertices[0].coordinate;
	const Coordinate& c1 = triangle->vertices[1].coordinate;
	const Coordinate& c2 = triangle->vertices[2].coordinate;

	int minX = FAST_MIN(c0.x, FAST_MIN(c1.x, c2.x));
	int maxX = FAST_MAX(c0.x, FAST_MAX(c1.x, c2.x));

	if (minX >= rasterWidth || maxX < 0) {
		// Optimize for horizontally offscreen triangles
		return false;
	}

	int minY = FAST_MIN(c0.y, FAST_MIN(c1.y, c2.y));
	int maxY = FAST_MAX(c0.y, FAST_MAX(c1.y, c2.y));

	// All we have left to check now is whether the
	// triangle is vertically on-screen
	return minY < rasterHeight && maxY > 0;
}

bool RasterFilter::isTriangleVisible(const Triangle* triangle) {
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

Triangle* RasterFilter::next() {
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
		Triangle* triangle = currentZone->at(currentElementIndex++);

		return isTriangleVisible(triangle) ? triangle : next();
	}
}

void RasterFilter::reset() {
	currentZoneIndex = 0;
	highestZoneIndex = 0;

	covers.clear();
}
