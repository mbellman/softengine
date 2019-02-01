#include <algorithm>
#include <optional>
#include <Graphics/RasterQueue.h>
#include <Helpers.h>

void RasterQueue::queue(Triangle triangle, int zoneIndex) {
	zoneIndex = clamp(zoneIndex, 0, RasterQueue::MAX_ZONES - 1);

	if (zoneIndex > highestZoneIndex) {
		highestZoneIndex = zoneIndex;
	}

	zones[zoneIndex].push_back(triangle);
}

Triangle* RasterQueue::next() {
	Zone* currentZone = &zones[currentZoneIndex];

	if (currentElementIndex >= currentZone->size() || currentZone->size() == 0) {
		currentZone->clear();

		currentElementIndex = 0;

		if (currentZoneIndex < highestZoneIndex) {
			currentZoneIndex++;

			return next();
		} else {
			currentZoneIndex = 0;
			highestZoneIndex = 0;

			return NULL;
		}
	} else {
		return &currentZone->at(currentElementIndex++);
	}
}
