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

Vec3 TriangleBuffer::getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex) {
	const Vertex2d& vertex = triangle->vertices[vertexIndex];
	const Settings& settings = activeLevel->getSettings();
	std::map<int, Vec3>& vertexLightCache = triangle->sourcePolygon->vertexLightCache[vertexIndex];
	Vec3 colorIntensity = { 1.0f, 1.0f, 1.0f };

	colorIntensity *= settings.brightness;

	if (settings.brightness == 0) {
		return colorIntensity;
	}

	// Ambient light is a special distance-invariant light
	// source which affects all geometry in the level
	if (settings.ambientLightFactor > 0) {
		const auto& cachedAmbientLight = vertexLightCache.find(TriangleBuffer::AMBIENT_LIGHT_ID);
		bool isCacheableLightSource = settings.hasStaticAmbientLight && triangle->sourceObject->isStatic && !triangle->isSynthetic;

		if (isCacheableLightSource && cachedAmbientLight != vertexLightCache.end()) {
			const Vec3& cachedAmbientLightIntensity = cachedAmbientLight->second;

			colorIntensity *= cachedAmbientLightIntensity;
		} else {
			float dot = Vec3::dotProduct(triangle->sourcePolygon->normal, settings.ambientLightVector.unit());

			if (dot < 0) {
				float incidence = cosf((1 + dot) * M_PI / 2);
				float intensity = incidence * settings.ambientLightFactor;
				const Vec3& colorRatios = settings.ambientLightColor.ratios();

				Vec3 ambientLightColorIntensity = {
					(1.0f + (intensity * colorRatios.x) / settings.brightness),
					(1.0f + (intensity * colorRatios.y) / settings.brightness),
					(1.0f + (intensity * colorRatios.z) / settings.brightness)
				};

				colorIntensity *= ambientLightColorIntensity;

				if (isCacheableLightSource) {
					vertexLightCache.emplace(TriangleBuffer::AMBIENT_LIGHT_ID, ambientLightColorIntensity);
				}
			}
		}
	}

	// Regular light sources must be within range of a vertex
	// to affect its color intensity
	for (const auto* light : activeLevel->getLights()) {
		bool isCacheableLightSource = light->isStatic && triangle->sourceObject->isStatic && !triangle->isSynthetic;

		if (isCacheableLightSource) {
			const auto& cachedLight = vertexLightCache.find(light->getId());

			if (cachedLight != vertexLightCache.end()) {
				const Vec3& cachedLightIntensity = cachedLight->second;

				colorIntensity *= cachedLightIntensity;

				continue;
			}
		}

		if (
			light->isDisabled ||
			light->power == 0 ||
			abs(light->position.x - vertex.worldVector.x) > light->range ||
			abs(light->position.y - vertex.worldVector.y) > light->range ||
			abs(light->position.z - vertex.worldVector.z) > light->range
		) {
			continue;
		}

		Vec3 lightVector = vertex.worldVector - light->position;
		float lightDistance = lightVector.magnitude();

		if (lightDistance < light->range) {
			float dot = Vec3::dotProduct(triangle->sourcePolygon->normal, lightVector.unit());

			if (dot < 0) {
				float incidence = cosf((1 + dot) * M_PI / 2);
				float illuminance = pow(1.0f - lightDistance / light->range, 2);
				float intensity = light->power * incidence * illuminance;
				const Vec3& colorRatios = light->getColorRatios();

				Vec3 lightColorIntensity = {
					(1.0f + (intensity * colorRatios.x) / settings.brightness),
					(1.0f + (intensity * colorRatios.y) / settings.brightness),
					(1.0f + (intensity * colorRatios.z) / settings.brightness)
				};

				colorIntensity *= lightColorIntensity;

				if (isCacheableLightSource) {
					vertexLightCache.emplace(light->getId(), lightColorIntensity);
				}
			}
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

void TriangleBuffer::illuminateTextureTriangle(Triangle* triangle) {
	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		vertex->textureIntensity = getTriangleVertexColorIntensity(triangle, i);
	}
}

void TriangleBuffer::illuminateTriangle(Triangle* triangle) {
	if (triangle->sourceObject->texture != NULL) {
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
