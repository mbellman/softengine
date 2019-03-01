#include <System/DebugStats.h>
#include <SDL.h>

/**
 * DebugStats
 * ----------
 */
void DebugStats::trackScreenProjectionTime() {
	screenProjectionTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackHiddenSurfaceRemovalTime() {
	hiddenSurfaceRemovalTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackIlluminationTime() {
	illuminationTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackDrawTime() {
	drawTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackUpdateTime() {
	updateTime.start = (int)SDL_GetTicks();
}

void DebugStats::trackFrameTime() {
	frameTime.start = (int)SDL_GetTicks();
}

void DebugStats::logScreenProjectionTime() {
	screenProjectionTime.end = (int)SDL_GetTicks();
}

void DebugStats::logHiddenSurfaceRemovalTime() {
	hiddenSurfaceRemovalTime.end = (int)SDL_GetTicks();
}

void DebugStats::logIlluminationTime() {
	illuminationTime.end = (int)SDL_GetTicks();
}

void DebugStats::logDrawTime() {
	drawTime.end = (int)SDL_GetTicks();
}

void DebugStats::logUpdateTime() {
	updateTime.end = (int)SDL_GetTicks();
}

void DebugStats::logFrameTime() {
	frameTime.end = (int)SDL_GetTicks();
}

int DebugStats::getScreenProjectionTime() {
	return screenProjectionTime.end - screenProjectionTime.start;
}

int DebugStats::getHiddenSurfaceRemovalTime() {
	return hiddenSurfaceRemovalTime.end - hiddenSurfaceRemovalTime.start;
}

int DebugStats::getIlluminationTime() {
	return illuminationTime.end - illuminationTime.start;
}

int DebugStats::getDrawTime() {
	return drawTime.end - drawTime.start;
}

int DebugStats::getUpdateTime() {
	return updateTime.end - updateTime.start;
}

int DebugStats::getFrameTime() {
	return frameTime.end - frameTime.start;
}

int DebugStats::getFPS() {
	return (int)(1000.0f / getFrameTime());
}

void DebugStats::countPolygons(int polygons) {
	totalPolygons += polygons;
}

void DebugStats::countVertices(int vertices) {
	totalVertices += vertices;
}

int DebugStats::getTotalPolygons(const std::vector<Object*>& objects) {
	return totalPolygons;
}

int DebugStats::getTotalVertices(const std::vector<Object*>& objects) {
	return totalVertices;
}

void DebugStats::reset() {
	totalPolygons = 0;
	totalVertices = 0;
}
