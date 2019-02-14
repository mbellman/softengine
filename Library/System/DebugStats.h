#pragma once

#include <System/Math.h>
#include <System/Objects.h>

class DebugStats {
public:
	void trackFrameTime();
	void trackScreenProjectionTime();
	void trackDrawTime();
	void logFrameTime();
	void logScreenProjectionTime();
	void logDrawTime();
	int getFrameTime();
	int getDrawTime();
	int getScreenProjectionTime();
	int getFPS();
	int getTotalPolygons(const std::vector<Object*>& objects);
	int getTotalVertices(const std::vector<Object*>& objects);
	int getTotalProjectedTriangles();
	int getTotalDrawnTriangles();
	void countProjectedTriangle();
	void countDrawnTriangle();
	void resetCounters();

private:
	Range<int> drawTime;
	Range<int> screenProjectionTime;
	Range<int> frameTime;

	int totalProjectedTriangles = 0;
	int totalDrawnTriangles = 0;
};
