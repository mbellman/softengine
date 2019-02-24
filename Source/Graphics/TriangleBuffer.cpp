#include <Graphics/TriangleBuffer.h>
#include <System/Geometry.h>
#include <System/Objects.h>
#include <System/Level.h>
#include <UI/Alert.h>
#include <Helpers.h>

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
	trianglePoolA = new Triangle[TriangleBuffer::TRIANGLE_POOL_SIZE];
	trianglePoolB = new Triangle[TriangleBuffer::TRIANGLE_POOL_SIZE];
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

void TriangleBuffer::computeAmbientLightColorIntensity(const Vec3& normal, Vec3& colorIntensity) {
	const Settings& settings = activeLevel->getSettings();

	if (settings.ambientLightFactor > 0) {
		float dot = Vec3::dotProduct(normal, settings.ambientLightVector.unit());

		if (dot < 0) {
			float incidence = cosf((1 + dot) * M_PI / 2);
			float intensity = incidence * settings.ambientLightFactor;
			const Vec3& colorRatios = settings.ambientLightColor.ratios();

			colorIntensity.x *= (1.0f + (intensity * colorRatios.x) / settings.brightness);
			colorIntensity.y *= (1.0f + (intensity * colorRatios.y) / settings.brightness);
			colorIntensity.z *= (1.0f + (intensity * colorRatios.z) / settings.brightness);
		}
	}
}

void TriangleBuffer::computeLightColorIntensity(const Light* light, const Vec3& vertexPosition, const Vec3& normal, Vec3& colorIntensity) {
	if (
		light->isDisabled ||
		light->power == 0 ||
		abs(light->position.x - vertexPosition.x) > light->range ||
		abs(light->position.y - vertexPosition.y) > light->range ||
		abs(light->position.z - vertexPosition.z) > light->range
	) {
		// Color intensity remains unaffected when lights
		// are disabled, at 0 power, or out of range.
		return;
	}

	const Settings& settings = activeLevel->getSettings();
	Vec3 lightVector = vertexPosition - light->position;
	float lightDistance = lightVector.magnitude();

	if (lightDistance < light->range) {
		float dot = Vec3::dotProduct(normal, lightVector.unit());

		if (dot < 0) {
			float incidence = cosf((1 + dot) * M_PI / 2);
			float illuminance = pow(1.0f - lightDistance / light->range, 2);
			float intensity = light->power * incidence * illuminance;
			const Vec3& colorRatios = light->getColorRatios();

			colorIntensity.x *= (1.0f + (intensity * colorRatios.x) / settings.brightness);
			colorIntensity.y *= (1.0f + (intensity * colorRatios.y) / settings.brightness);
			colorIntensity.z *= (1.0f + (intensity * colorRatios.z) / settings.brightness);
		}
	}
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

Vec3 TriangleBuffer::getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex) {
	const Vertex2d& vertex = triangle->vertices[vertexIndex];
	const Settings& settings = activeLevel->getSettings();
	bool isStaticTriangle = !triangle->isSynthetic && triangle->sourcePolygon->sourceObject->isStatic;
	Vec3 colorIntensity;

	if (isStaticTriangle) {
		colorIntensity = triangle->sourcePolygon->cachedVertexColorIntensities[vertexIndex];
	} else {
		colorIntensity = { settings.brightness, settings.brightness, settings.brightness };
	}

	if (settings.brightness == 0) {
		return colorIntensity;
	}

	if (settings.ambientLightFactor > 0) {
		bool shouldRecomputeAmbientLightColorIntensity = !isStaticTriangle || !settings.hasStaticAmbientLight;

		if (shouldRecomputeAmbientLightColorIntensity) {
			computeAmbientLightColorIntensity(triangle->sourcePolygon->normal, colorIntensity);
		}
	}

	for (const auto* light : activeLevel->getLights()) {
		bool shouldRecomputeLightColorIntensity = !isStaticTriangle || !light->isStatic;

		if (shouldRecomputeLightColorIntensity) {
			computeLightColorIntensity(light, vertex.worldVector, triangle->sourcePolygon->normal, colorIntensity);
		}
	}

	return colorIntensity;
}

void TriangleBuffer::illuminateColorTriangle(Triangle* triangle) {
	const Settings& settings = activeLevel->getSettings();

	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		const Vec3 colorIntensity = getTriangleVertexColorIntensity(triangle, i);

		vertex->color.R *= colorIntensity.x;
		vertex->color.G *= colorIntensity.y;
		vertex->color.B *= colorIntensity.z;
		vertex->color.clamp();

		float visibilityRatio = FAST_MIN(vertex->z / settings.visibility, 1.0f);

		vertex->color = Color::lerp(vertex->color, settings.backgroundColor, visibilityRatio);
	}
}

/**
 * Performs a one-time illumination step on Polygons belonging to
 * static Objects, storing the color intensity results in the
 * Polygon's vertex color intensity cache. Only static ambient
 * light (if applicable) and static light sources should factor
 * into the cached value; non-static light sources must be
 * recalculated during runtime.
 */
void TriangleBuffer::illuminateStaticPolygon(Polygon* polygon) {
	const Settings& settings = activeLevel->getSettings();

	for (int i = 0; i < 3; i++) {
		Vec3 vertexPosition = polygon->sourceObject->position + polygon->vertices[i]->vector;
		Vec3 colorIntensity = { settings.brightness, settings.brightness, settings.brightness };
		Vec3& cachedColorIntensity = polygon->cachedVertexColorIntensities[i];

		if (settings.hasStaticAmbientLight) {
			computeAmbientLightColorIntensity(polygon->normal, colorIntensity);
		}

		for (auto* light : activeLevel->getLights()) {
			if (light->isStatic) {
				computeLightColorIntensity(light, vertexPosition, polygon->normal, colorIntensity);
			}
		}

		cachedColorIntensity.x = colorIntensity.x;
		cachedColorIntensity.y = colorIntensity.y;
		cachedColorIntensity.z = colorIntensity.z;
	}
}

void TriangleBuffer::illuminateTextureTriangle(Triangle* triangle) {
	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		vertex->textureIntensity = getTriangleVertexColorIntensity(triangle, i);
	}
}

void TriangleBuffer::illuminateTriangle(Triangle* triangle) {
	if (triangle->sourcePolygon->sourceObject->texture != NULL) {
		illuminateTextureTriangle(triangle);
	} else {
		illuminateColorTriangle(triangle);
	}
}

Triangle* TriangleBuffer::requestTriangle() {
	if (totalRequestedTriangles >= TriangleBuffer::TRIANGLE_POOL_SIZE) {
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

void TriangleBuffer::setActiveLevel(Level* activeLevel) {
	this->activeLevel = activeLevel;
}
