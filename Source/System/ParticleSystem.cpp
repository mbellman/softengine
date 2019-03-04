#include <System/ParticleSystem.h>
#include <System/Objects.h>
#include <Graphics/Color.h>
#include <Graphics/TextureBuffer.h>
#include <Helpers.h>
#include <functional>

/**
 * ParticleSystem
 * --------------
 */
ParticleSystem::ParticleSystem(int size) {
	for (int i = 0; i < size; i++) {
		particles.push_back(new Particle());
	}
}

ParticleSystem::~ParticleSystem() {
	for (auto* particle : particles) {
		delete particle;
	}

	particles.clear();
}

const std::vector<Particle*>& ParticleSystem::getParticles() const {
	return particles;
}

void ParticleSystem::resetParticle(Particle* particle) {
	particle->position = {
		position.x + RNG::random(xSpawnRange.start, xSpawnRange.end),
		position.y + RNG::random(ySpawnRange.start, ySpawnRange.end),
		position.z + RNG::random(zSpawnRange.start, zSpawnRange.end)
	};

	particle->rotateDeg({ 0.0f, RNG::random(0.0f, 360.0f), 0.0f });
	particle->shouldReset = false;
}

void ParticleSystem::setParticleBehavior(std::function<void(Particle*, int)> handler) {
	behaviorHandler = handler;
}

void ParticleSystem::setParticleColor(const Color& color) {
	for (auto* particle : particles) {
		particle->setColor(color);
	}
}

void ParticleSystem::setParticleSize(float width, float height) {
	for (auto* particle : particles) {
		particle->scale({ width / 2.0f, height / 2.0f, 1.0f });
	}
}

void ParticleSystem::setParticleTexture(TextureBuffer* texture) {
	for (auto* particle : particles) {
		particle->setTexture(texture);
	}
}

void ParticleSystem::setSpawnLocation(const Vec3& location) {
	xSpawnRange.start = location.x;
	xSpawnRange.end = location.x;

	ySpawnRange.start = location.y;
	ySpawnRange.end = location.y;

	zSpawnRange.start = location.z;
	zSpawnRange.end = location.z;
}

void ParticleSystem::setSpawnRange(const Range<float> xSpawnRange, const Range<float> ySpawnRange, const Range<float> zSpawnRange) {
	this->xSpawnRange = xSpawnRange;
	this->ySpawnRange = ySpawnRange;
	this->zSpawnRange = zSpawnRange;
}

void ParticleSystem::update(int dt) {
	for (auto* particle : particles) {
		if (particle->shouldReset) {
			resetParticle(particle);
		}

		behaviorHandler(particle, dt);
	}

	updatePosition(dt);
}
