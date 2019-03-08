#include <System/Camera.h>
#include <System/Math.h>
#include <System/Quaternion.h>

/**
 * Camera
 * ------
 */
RotationMatrix Camera::getRotationMatrix() const {
	Quaternion q1 = Quaternion::fromAxisAngle(pitch, 1, 0, 0);
	Quaternion q2 = Quaternion::fromAxisAngle(yaw, 0, 1, 0);
	Quaternion q3 = Quaternion::fromAxisAngle(roll, 0, 0, 1);

	return (q1 * q2 * q3).toRotationMatrix();
}

void Camera::update(int dt) {
	updatePosition(dt);
}
