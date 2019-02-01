#pragma once

#include <vector>
#include <optional>
#include <Types.h>

typedef std::vector<Triangle> Zone;

class RasterQueue {
public:
	bool isEmpty();
	void queue(Triangle triangle, int zoneIndex);
	Triangle* next();

private:
	constexpr static int MAX_ZONES = 50;
	int currentZoneIndex = 0;
	int highestZoneIndex = 0;
	int currentElementIndex = 0;
	Zone zones[RasterQueue::MAX_ZONES];
};
