#include <math.h>
#include <Types.h>
#include <Quaternion.h>

Quaternion Quaternion::fromAxisAngle(float angle, float xi, float yj, float zk) {
	float sa = sinf(angle / 2);

	return {
		cosf(angle / 2),
		xi * sa,
		yj * sa,
		zk * sa
	};
}

Quaternion Quaternion::operator *(const Quaternion& q2) const {
	return {
		w * q2.w - x * q2.x - y * q2.y - z * q2.z,
		w * q2.x + x * q2.w + y * q2.z - z * q2.y,
		w * q2.y - x * q2.z + y * q2.w + z * q2.x,
		w * q2.z + x * q2.y - y * q2.x + z * q2.w
	};
}

RotationMatrix Quaternion::toRotationMatrix() {
	return {
		1 - 2 * y * y - 2 * z * z, 2 * x * y - 2 * z * w, 2 * x * z + 2 * y * w,
		2 * x * y + 2 * z * w, 1 - 2 * x * x - 2 * z * z, 2 * y * z - 2 * x * w,
		2 * x * z - 2 * y * w, 2 * y * z + 2 * x * w, 1 - 2 * x * x - 2 * y * y
	};
}
