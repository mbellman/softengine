#include <System/Camera.h>
#include <System/Math.h>
#include <System/Quaternion.h>

/**
 * Camera
 * ------
 */
RotationMatrix Camera::getRotationMatrix() {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);

	return (q1 * q2).toRotationMatrix();
}

void Camera::update() {
	updatePosition();
}
