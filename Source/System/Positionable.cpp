#include <System/Positionable.h>

/**
 * Positionable
 * ------------
 */
Positionable::Positionable() {}

Positionable::Positionable(const Vec3& position) {
	this->position = position;
}

void Positionable::follow(const Positionable* target, PositionHandler handler) {
	positionTarget = target;
	positionHandler = handler;
}

void Positionable::updatePosition() {
	if (positionTarget != NULL) {
		positionHandler(positionTarget->position, position);
	}
}
