#include <System/Camera.h>
#include <System/Math.h>
#include <System/Quaternion.h>
#include <Constants.h>
#include <Helpers.h>
#include <cmath>
#include <math.h>
#include <SDL.h>

/**
 * Camera
 * ------
 */
Vec3 Camera::getDirection() const {
	Vec3 direction;
	float pitchFactor = std::fabs(cosf(pitch));

	direction.x = -sinf(yaw) * pitchFactor;
	direction.y = sinf(pitch);
	direction.z = cosf(yaw) * pitchFactor;

	return direction.unit();
}

RotationMatrix Camera::getRotationMatrix() const {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);
	Quaternion q3 = Quaternion::fromAxisAngle(roll, 0, 0, 1);

	return (q1 * q2 * q3).toRotationMatrix();
}

void Camera::lookAt(const Positionable3d* positionable) {
	setDirection(positionable->position - position);
}

void Camera::setDirection(const Vec3& direction) {
	float directionalPitch = asinf(direction.unit().y);
	float directionalYaw = std::atan2(direction.z, direction.x) - PI_HALF;

	pitch = FAST_CLAMP(directionalPitch, -MAX_CAMERA_PITCH, MAX_CAMERA_PITCH);
	yaw = directionalYaw;
}

void Camera::update(int dt) {
	updatePosition(dt);
}
