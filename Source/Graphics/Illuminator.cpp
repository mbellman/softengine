#include <Graphics/Illuminator.h>
#include <System/Math.h>
#include <System/Geometry.h>
#include <Helpers.h>
#include <SDL.h>

constexpr static float PI_HALF = M_PI / 2.0f;

/**
 * Illuminator
 * -----------
 */
void Illuminator::computeAmbientLightColorIntensity(const Vec3& normal, float fresnelFactor, Vec3& colorIntensity) {
	const Settings& settings = activeScene->settings;

	if (settings.ambientLightFactor > 0) {
		float dot = Vec3::dotProduct(normal, settings.ambientLightVector.unit());

		if (dot < 0) {
			float incidence = cosf((1 + dot) * M_PI / 2);
			float intensity = incidence * settings.ambientLightFactor * (1.0f + fresnelFactor);
			const Vec3& colorRatios = settings.ambientLightColor.ratios();

			colorIntensity.x *= (1.0f + (intensity * colorRatios.x) / settings.brightness);
			colorIntensity.y *= (1.0f + (intensity * colorRatios.y) / settings.brightness);
			colorIntensity.z *= (1.0f + (intensity * colorRatios.z) / settings.brightness);
		}
	}
}

void Illuminator::computeLightColorIntensity(Light* light, const Vec3& vertexPosition, const Vec3& normal, float fresnelFactor, Vec3& colorIntensity) {
	if (
		light->isDisabled ||
		light->power == 0 ||
		abs(light->position.x - vertexPosition.x) > light->range ||
		abs(light->position.y - vertexPosition.y) > light->range ||
		abs(light->position.z - vertexPosition.z) > light->range
	) {
		// Color intensity remains unaffected when lights
		// are disabled, at 0 power, or out of axial range.
		return;
	}

	const Settings& settings = activeScene->settings;
	Vec3 lightSourceVector = vertexPosition - light->position;
	float lightDistance = lightSourceVector.magnitude();

	if (lightDistance > light->range) {
		return;
	}

	// Normalize for dot product checks
	lightSourceVector /= lightDistance;

	float normalDot = Vec3::dotProduct(normal, lightSourceVector);

	if (normalDot >= 0) {
		// Ignore vertices facing away from the light
		return;
	}

	bool isDirectional = light->isOfType<DirectionalLight>();

	// Directional lights use the angle between the light direction
	// and the light-to-vertex vector to compute incidence. In this
	// case we flip the light source vector sign to point it toward
	// the light, rather than toward the vertex, ensuring consistency
	// with the standard vertex normal dot product comparison.
	float directionalDot = isDirectional
		? Vec3::dotProduct(((DirectionalLight*)light)->getDirection(), lightSourceVector * -1.0f)
		: 0.0f;

	if (isDirectional && directionalDot >= 0) {
		// For directional lights, ignore vertices
		// behind the light direction vector
		return;
	}

	float incidence = getIncidence(normalDot) * (isDirectional ? powf(directionalDot, 4) : 1.0f);
	float illuminance = pow(1.0f - lightDistance / light->range, 2);
	float intensity = light->power * incidence * illuminance * (1.0f + fresnelFactor);
	const Vec3& colorRatios = light->getColorRatios();

	colorIntensity.x *= (1.0f + (intensity * colorRatios.x) / settings.brightness);
	colorIntensity.y *= (1.0f + (intensity * colorRatios.y) / settings.brightness);
	colorIntensity.z *= (1.0f + (intensity * colorRatios.z) / settings.brightness);
}

inline float Illuminator::getIncidence(float dot) {
	return cosf((1 + dot) * PI_HALF);
}

Vec3 Illuminator::getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex) {
	const Vertex2d& vertex = triangle->vertices[vertexIndex];
	const Vec3& normal = triangle->sourcePolygon->sourceObject->isFlatShaded ? triangle->sourcePolygon->normal : vertex.normal;
	const Settings& settings = activeScene->settings;
	bool isStaticTriangle = !triangle->isSynthetic && triangle->sourcePolygon->sourceObject->isStatic;
	Vec3 colorIntensity;

	if (isStaticTriangle) {
		colorIntensity = triangle->sourcePolygon->cachedVertexColorIntensities[vertexIndex];
	} else {
		colorIntensity = { settings.brightness, settings.brightness, settings.brightness };
	}

	if (settings.brightness > 0.0f) {
		bool shouldRecomputeAmbientLightColorIntensity = settings.ambientLightFactor > 0 && (!isStaticTriangle || !settings.hasStaticAmbientLight);

		if (shouldRecomputeAmbientLightColorIntensity) {
			computeAmbientLightColorIntensity(normal, triangle->fresnelFactor, colorIntensity);
		}

		for (auto* light : activeScene->getLights()) {
			bool shouldRecomputeLightColorIntensity = !isStaticTriangle || !light->isStatic;

			if (shouldRecomputeLightColorIntensity) {
				computeLightColorIntensity(light, vertex.worldVector, normal, triangle->fresnelFactor, colorIntensity);
			}
		}
	}

	return colorIntensity;
}

void Illuminator::illuminateColorTriangle(Triangle* triangle) {
	const Settings& settings = activeScene->settings;

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
void Illuminator::illuminateStaticPolygon(Polygon* polygon) {
	const Settings& settings = activeScene->settings;
	float fresnelFactor = 0.0f;

	for (int i = 0; i < 3; i++) {
		Vec3 vertexPosition = polygon->sourceObject->position + polygon->vertices[i]->vector;
		Vec3& normal = polygon->sourceObject->isFlatShaded ? polygon->normal : polygon->vertices[i]->normal;
		Vec3 colorIntensity = { settings.brightness, settings.brightness, settings.brightness };
		Vec3& cachedColorIntensity = polygon->cachedVertexColorIntensities[i];

		if (settings.hasStaticAmbientLight && settings.ambientLightFactor > 0) {
			computeAmbientLightColorIntensity(normal, fresnelFactor, colorIntensity);
		}

		for (auto* light : activeScene->getLights()) {
			if (light->isStatic) {
				computeLightColorIntensity(light, vertexPosition, normal, fresnelFactor, colorIntensity);
			}
		}

		cachedColorIntensity.x = colorIntensity.x;
		cachedColorIntensity.y = colorIntensity.y;
		cachedColorIntensity.z = colorIntensity.z;
	}
}

void Illuminator::illuminateTextureTriangle(Triangle* triangle) {
	for (int i = 0; i < 3; i++) {
		Vertex2d* vertex = &triangle->vertices[i];
		vertex->textureIntensity = getTriangleVertexColorIntensity(triangle, i);
	}
}

void Illuminator::illuminateTriangle(Triangle* triangle) {
	if (!triangle->sourcePolygon->sourceObject->hasLighting) {
		// Clear any previous lighting values, since
		// Triangles are recycled from the pool
		resetTriangleLighting(triangle);

		return;
	}

	if (triangle->sourcePolygon->sourceObject->texture != NULL) {
		illuminateTextureTriangle(triangle);
	} else {
		illuminateColorTriangle(triangle);
	}
}

void Illuminator::resetTriangleLighting(Triangle* triangle) {
	for (int i = 0; i < 3; i++) {
		Vertex2d& vertex = triangle->vertices[i];

		vertex.textureIntensity.x = 1.0f;
		vertex.textureIntensity.y = 1.0f;
		vertex.textureIntensity.z = 1.0f;
	}
}

void Illuminator::setActiveScene(Scene* scene) {
	activeScene = scene;
}
