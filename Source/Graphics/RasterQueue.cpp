#include <algorithm>
#include <optional>
#include <Helpers.h>
#include <Graphics/RasterQueue.h>

RasterQueue::RasterQueue(int width, int height) {
	this->width = width;
	this->height = height;
}

void RasterQueue::addCover(Triangle& triangle, int zone) {
	covers.push_back({
		triangle.vertices[0].coordinate,
		triangle.vertices[1].coordinate,
		triangle.vertices[2].coordinate,
		zone
	});
}

void RasterQueue::addTriangle(Triangle triangle, int zoneIndex) {
	zoneIndex = clamp(zoneIndex, 0, RasterQueue::MAX_ZONES - 1);

	if (zoneIndex > highestZoneIndex) {
		highestZoneIndex = zoneIndex;
	}

	if (isTriangleCoverable(triangle)) {
		addCover(triangle, zoneIndex);
	}

	zones[zoneIndex].push_back(triangle);
}

int RasterQueue::DEBUG_getTotalCovers() {
	return covers.size();
}

bool RasterQueue::isPointWithinEdge(int x, int y, int ex1, int ey1, int ex2, int ey2) {
	if (ex1 == ex2) {
		// Trivial case #1: For vertical edges, we can simply
		// check whether the point x is > the edge x for downward
		// lines or < the edge x for upward lines.
		return ey2 > ey1 ? (x > ex1) : (x < ex1);
	} else if (ey1 == ey2) {
		// Trivial case #2: For horizontal edges, we can simply
		// check whether the point y is < the edge y for rightward
		// lines or > the edge y for leftward lines.
		return ex2 > ex1 ? (y < ey1) : (y > ey1);
	} else {
		// Nontrivial case: we have to determine the edge slope
		// and check whether the point is 'inside' or 'outside'.
		float slope = (float)(ey2 - ey1) / (ex2 - ex1);
		bool isDownwardEdge = ey2 > ey1;
		int edgeXAtPoint = ex1 + (int)((y - ey1) / slope);

		return isDownwardEdge ? (x > edgeXAtPoint) : (x < edgeXAtPoint);
	}
}

bool RasterQueue::isTriangleCoverable(Triangle& triangle) {
	Coordinate* c1 = &triangle.vertices[0].coordinate;
	Coordinate* c2 = &triangle.vertices[1].coordinate;
	Coordinate* c3 = &triangle.vertices[2].coordinate;

	int minX = std::min(c1->x, std::min(c2->x, c3->x));
	int maxX = std::max(c1->x, std::max(c2->x, c3->x));

	int minY = std::min(c1->y, std::min(c2->y, c3->y));
	int maxY = std::max(c1->y, std::max(c2->y, c3->y));

	return (
		// Ensure that the triangle is over the minimum
		// cover size along both axes
		(maxX - minX) > MIN_COVER_SIZE &&
		(maxY - minY) > MIN_COVER_SIZE &&
		// Ensure that it extends far enough into the screen
		// that distant triangles are likely to be covered by it
		(minX < (width - MIN_COVER_SIZE) && maxX > MIN_COVER_SIZE) &&
		(minY < (height - MIN_COVER_SIZE) && maxY > MIN_COVER_SIZE)
	);
}

/**
 * Determines whether a Triangle is occluded by a Cover.
 */
bool RasterQueue::isTriangleOccluded(Triangle& triangle, const Cover& cover) {
	for (int i = 0; i < 3; i++) {
		Coordinate* v = &triangle.vertices[i].coordinate;

		// To determine whether a triangle T is completely covered by
		// another triangle T*, we have to check T's vertices against
		// T*'s edges. Whereas in world space polygon vertices are
		// oriented counter-clockwise, in raster space they are oriented
		// clockwise due to the inversion of the y-axis. In order to
		// correctly determine which 'side' of T*'s edges T's vertices
		// lie on, we need to check the edges along T*v1 -> T*v3,
		// T*v3 -> T*v2, and T*v2 -> T*v1.
		if (
			!isPointWithinEdge(v->x, v->y, cover.c1.x, cover.c1.y, cover.c3.x, cover.c3.y) ||
			!isPointWithinEdge(v->x, v->y, cover.c3.x, cover.c3.y, cover.c2.x, cover.c2.y) ||
			!isPointWithinEdge(v->x, v->y, cover.c2.x, cover.c2.y, cover.c1.x, cover.c1.y)
		) {
			return false;
		}
	}

	return true;
}

bool RasterQueue::isTriangleVisible(Triangle& triangle) {
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
