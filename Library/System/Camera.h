#pragma once

#include <System/Math.h>
#include <System/Positionable.h>
#include <SDL.h>

/**
 * Camera
 * ------
 */
struct Camera : public Positionable3d {
	constexpr static float MAX_PITCH = 89 * M_PI / 180;
	float pitch = 0.0f;
	float yaw = 0.0f;
	int fov = 90;

	Camera(): Positionable3d({ 0, 100, 0 }) {}

	RotationMatrix getRotationMatrix();
	void update(int dt);
};
