#pragma once

#include <functional>
#include <System/Math.h>

typedef std::function<void(const Vec3&, Vec3&)> FollowHandler;

/**
 * Positionable2d
 * --------------
 */
struct Positionable2d {
	Coordinate position;

	Positionable2d();
	Positionable2d(const Coordinate& position);

	void tweenTo(const Coordinate& target, int duration, Ease::EaseFunction easeFunction);

protected:
	void updatePosition(int dt);

private:
	Tween<Coordinate> tween;
};

/**
 * Positionable3d
 * --------------
 */
struct Positionable3d {
	Vec3 position;

	Positionable3d();
	Positionable3d(const Vec3& position);

	void follow(const Positionable3d* target, FollowHandler handler);
	void lockTo(const Positionable3d* target);
	void tweenTo(const Vec3& target, int duration, Ease::EaseFunction easeFunction);

protected:
	void updatePosition(int dt);

private:
	const Positionable3d* followTarget = nullptr;
	FollowHandler followHandler;
	Tween<Vec3> tween;
};
