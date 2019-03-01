#pragma once

#include <System/Math.h>
#include <System/Objects.h>

class DebugStats {
public:
	void trackScreenProjectionTime();
	void trackHiddenSurfaceRemovalTime();
	void trackIlluminationTime();
	void trackDrawTime();
	void trackUpdateTime();
	void trackFrameTime();
	void logScreenProjectionTime();
	void logHiddenSurfaceRemovalTime();
	void logIlluminationTime();
	void logDrawTime();
	void logUpdateTime();
	void logFrameTime();
	int getScreenProjectionTime();
	int getHiddenSurfaceRemovalTime();
	int getIlluminationTime();
	int getDrawTime();
	int getUpdateTime();
	int getFrameTime();
	int getFPS();
	void countPolygons(int polygons);
	void countVertices(int vertices);
	int getTotalPolygons(const std::vector<Object*>& objects);
	int getTotalVertices(const std::vector<Object*>& objects);
	void reset();

private:
	Range<int> screenProjectionTime;
	Range<int> hiddenSurfaceRemovalTime;
	Range<int> illuminationTime;
	Range<int> drawTime;
	Range<int> updateTime;
	Range<int> frameTime;

	int totalPolygons = 0;
	int totalVertices = 0;
};
