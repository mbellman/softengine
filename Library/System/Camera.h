#pragma once

#include <System/Math.h>
#include <System/Positionable.h>

/**
 * Camera
 * ------
 */
struct Camera : public Positionable3d {
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	int fov = 90;

	Camera(): Positionable3d({ 0, 100, 0 }) {}

	RotationMatrix getRotationMatrix() const;
	void update(int dt);
};
