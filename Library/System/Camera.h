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
	float movementSpeed = 5.0f;
	float mouseSensitivity = 1.0f;

	Camera(): Positionable3d({ 0, 100, 0 }) {}

	Vec3 getDirection() const;
	RotationMatrix getRotationMatrix() const;
	void lookAt(const Positionable3d* positionable);
	void setDirection(const Vec3& direction);
	void update(int dt);
};
