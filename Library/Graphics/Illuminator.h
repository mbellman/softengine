#pragma once

#include <System/Scene.h>
#include <System/Math.h>
#include <System/Geometry.h>

/**
 * Illuminator
 * -----------
 */
class Illuminator {
public:
	void computeAmbientLightColorIntensity(const Vec3& vertexNormal, float fresnelFactor, Vec3& colorIntensity);
	void computeLightColorIntensity(Light* light, const Vec3& vertexPosition, const Vec3& vertexNormal, float fresnelFactor, Vec3& colorIntensity);
	void illuminateTriangle(Triangle* triangle);
	void illuminateStaticPolygon(Polygon* polygon);
	void setActiveScene(Scene* scene);

private:
	Scene* activeScene = 0;

	float getIncidence(float dot);
	Vec3 getTriangleVertexColorIntensity(Triangle* triangle, int vertexIndex);
	void illuminateColorTriangle(Triangle* triangle);
	void illuminateTextureTriangle(Triangle* triangle);
	void resetTriangleLighting(Triangle* triangle);
};
