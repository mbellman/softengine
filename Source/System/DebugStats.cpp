#include <System/DebugStats.h>
#include <SDL.h>

/**
 * DebugStats
 * ----------
 */
void DebugStats::trackScreenProjectionTime() {
	screenProjectionTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackIlluminationTime() {
	illuminationTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackDrawTime() {
	drawTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackFrameTime() {
	frameTime.start = (int)SDL_GetTicks();
}

void DebugStats::logScreenProjectionTime() {
	screenProjectionTime.end = (int)SDL_GetTicks();
}

void DebugStats::logIlluminationTime() {
	illuminationTime.end = (int)SDL_GetTicks();
}

void DebugStats::logDrawTime() {
	drawTime.end = (int)SDL_GetTicks();
}

void DebugStats::logFrameTime() {
	frameTime.end = (int)SDL_GetTicks();
}

int DebugStats::getScreenProjectionTime() {
	return screenProjectionTime.end - screenProjectionTime.start;
}

int DebugStats::getIlluminationTime() {
	return illuminationTime.end - illuminationTime.start;
}

int DebugStats::getDrawTime() {
	return drawTime.end - drawTime.start;
}

int DebugStats::getFrameTime() {
	return frameTime.end - frameTime.start;
}

int DebugStats::getFPS() {
	return (int)(1000.0f / getFrameTime());
}

int DebugStats::getTotalPolygons(const std::vector<Object*>& objects) {
	int total = 0;

	for (const Object* object : objects) {
		total += object->getPolygonCount();
	}

	return total;
}

int DebugStats::getTotalVertices(const std::vector<Object*>& objects) {
	int total = 0;

	for (const Object* object : objects) {
		total += object->getVertexCount();
	}

	return total;
}

int DebugStats::getTotalDrawnTriangles() {
	return totalDrawnTriangles;
}

int DebugStats::getTotalProjectedTriangles() {
	return totalProjectedTriangles;
}

void DebugStats::countDrawnTriangle() {
	totalDrawnTriangles++;
}

void DebugStats::countProjectedTriangle() {
	totalProjectedTriangles++;
}

void DebugStats::resetCounters() {
	totalDrawnTriangles = 0;
	totalProjectedTriangles = 0;
}
