#pragma once

#include <functional>
#include <System/Math.h>

typedef std::function<void(const Vec3&, Vec3&)> PositionHandler;

/**
 * Positionable
 * ------------
 */
struct Positionable {
	Vec3 position;

	Positionable();
	Positionable(const Vec3& position);

	void follow(const Positionable* target, PositionHandler handler);

protected:
	const Positionable* positionTarget = 0;
	PositionHandler positionHandler;

	void updatePosition();
};
