#include <System/Positionable.h>
#include <System/Math.h>
#include <Helpers.h>

/**
 * Positionable2d
 * --------------
 */
Positionable2d::Positionable2d() {}

Positionable2d::Positionable2d(const Coordinate& position) {
	this->position = position;
}

void Positionable2d::tweenTo(const Coordinate& target, int duration, Ease::EaseFunction easing) {
	tween.value.start = position;
	tween.value.end = target;
	tween.duration = duration;
	tween.time = 0;
	tween.easing = easing;
	tween.isActive = true;
}

void Positionable2d::updatePosition(int dt) {
	if (tween.isActive) {
		tween.time += dt;

		float alpha = tween.alpha();

		if (alpha == 1.0f) {
			tween.isActive = false;
		}

		position = Coordinate::lerp(tween.value.start, tween.value.end, alpha);
	}
}

/**
 * Positionable3d
 * --------------
 */
Positionable3d::Positionable3d() {}

Positionable3d::Positionable3d(const Vec3& position) {
	this->position = position;
}

void Positionable3d::follow(const Positionable3d* target, FollowHandler handler) {
	followTarget = target;
	followHandler = handler;
}

void Positionable3d::tweenTo(const Vec3& target, int duration, Ease::EaseFunction easing) {
	tween.value.start = position;
	tween.value.end = target;
	tween.duration = duration;
	tween.time = 0;
	tween.easing = easing;
	tween.isActive = true;
}

void Positionable3d::updatePosition(int dt) {
	if (followTarget != nullptr) {
		followHandler(followTarget->position, position);
	} else if (tween.isActive) {
		tween.time += dt;

		float alpha = tween.alpha();

		if (alpha == 1.0f) {
			tween.isActive = false;
		}

		position = Vec3::lerp(tween.value.start, tween.value.end, alpha);
	}
}
