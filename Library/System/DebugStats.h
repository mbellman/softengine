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
	int getTotalPolygons(const std::vector<Object*>& objects);
	int getTotalVertices(const std::vector<Object*>& objects);

private:
	Range<int> screenProjectionTime;
	Range<int> hiddenSurfaceRemovalTime;
	Range<int> illuminationTime;
	Range<int> drawTime;
	Range<int> updateTime;
	Range<int> frameTime;

	int totalProjectedTriangles = 0;
	int totalDrawnTriangles = 0;
};
