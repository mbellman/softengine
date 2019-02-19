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
	Vec3 newPosition = {
		RNG::random(range[0].x, range[1].x),
		RNG::random(range[0].y, range[1].y),
		RNG::random(range[0].z, range[1].z)
	};

	particle->position = newPosition;
	particle->shouldReset = false;
}

void ParticleSystem::setBehavior(std::function<void(Particle*, int)> handler) {
	behaviorHandler = handler;
}

void ParticleSystem::setLocation(const Vec3 location) {
	setRange(location, location);
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

void ParticleSystem::setRange(const Vec3 r1, const Vec3 r2) {
	range[0] = r1;
	range[1] = r2;
}

void ParticleSystem::update(int dt) {
	for (auto* particle : particles) {
		if (particle->shouldReset) {
			resetParticle(particle);
		}

		behaviorHandler(particle, dt);
	}
}
