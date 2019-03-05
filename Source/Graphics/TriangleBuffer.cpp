#include <Graphics/TriangleBuffer.h>
#include <System/Geometry.h>
#include <System/Objects.h>
#include <System/Level.h>
#include <UI/Alert.h>
#include <Helpers.h>
#include <Constants.h>

/**
 * TriangleBuffer
 * --------------
 *
 * Provides a reusable pool of Triangle objects for use by the
 * main engine, and methods for applying visual effects preceding
 * actual rasterization. Two pools are available, and two buffers
 * for queueing Triangles once qualified for rasterization. To
 * support the engine's multithreaded architecture, the alternate
 * pools and buffers allow next-frame screen projection to occur
 * while previous-frame rendering occurs simultaneously. Since
 * screen projection requires recycling Triangles, and render
 * effects require mutating them (for example, altering vertex
 * color information), the separate pools and buffers allow us to
 * swap back and forth on each frame so that a Triangle is never
 * simultaneously mutated, or buffers simultaneously written/read,
 * by different threads.
 *
 * In single-threaded mode, the pool/buffer swapping still occurs,
 * with virtually no cost, but no utility either.
 */
TriangleBuffer::TriangleBuffer() {
	trianglePoolA = new Triangle[TRIANGLE_POOL_SIZE];
	trianglePoolB = new Triangle[TRIANGLE_POOL_SIZE];
}

TriangleBuffer::~TriangleBuffer() {
	delete[] trianglePoolA;
	delete[] trianglePoolB;

	triangleBufferA.clear();
	triangleBufferB.clear();
}

/**
 * Places a screen-projected Triangle into the primary buffer.
 * Once the buffers are swapped at the end of the frame and
 * the primary buffer becomes secondary, the rendering pipeline
 * can consume and apply effects to Triangles in the buffer
 * while the screen projection/raster filter pipeline queues
 * new Triangles into the new primary buffer.
 */
void TriangleBuffer::bufferTriangle(Triangle* triangle) {
	auto& primaryBuffer = isSwapped ? triangleBufferB : triangleBufferA;

	primaryBuffer.push_back(triangle);
}

/**
 * Returns the secondary triangle buffer for consumption by the
 * rendering pipeline, after it has already been written to by
 * the screen projection/raster filter pipeline.
 */
const std::vector<Triangle*>& TriangleBuffer::getBufferedTriangles() {
	auto& secondaryBuffer = isSwapped ? triangleBufferA : triangleBufferB;

	return secondaryBuffer;
}

int TriangleBuffer::getTotalRequestedTriangles() {
	return totalRequestedTriangles;
}

int TriangleBuffer::getTotalNonStaticTriangles() {
	int total = 0;

	for (auto* triangle : getBufferedTriangles()) {
		if (!triangle->sourcePolygon->sourceObject->isStatic) {
			total++;
		}
	}

	return total;
}

Triangle* TriangleBuffer::requestTriangle() {
	if (totalRequestedTriangles >= TRIANGLE_POOL_SIZE) {
		Alert::error(ALERT_ERROR, "Triangle buffer overflow");
		exit(0);
	}

	Triangle* pool = isSwapped ? trianglePoolB : trianglePoolA;

	return &pool[totalRequestedTriangles++];
}

/**
 * Resets state by A) resetting the requested triangle counter,
 * B) swapping the primary and secondary pools/buffers, and
 * C) clearing the new primary buffer (previously filled with
 * render-ready Triangles) so it can be written to with new
 * screen-projected Triangles on the next frame.
 */
void TriangleBuffer::reset() {
	totalRequestedTriangles = 0;
	isSwapped = !isSwapped;

	auto& primaryBuffer = isSwapped ? triangleBufferB : triangleBufferA;

	primaryBuffer.clear();
}
