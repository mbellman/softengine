#pragma once

#include <System/Objects.h>
#include <System/Positionable.h>
#include <Graphics/Color.h>
#include <Graphics/TextureBuffer.h>
#include <System/Math.h>
#include <functional>
#include <vector>

/**
 * ParticleSystem
 * --------------
 */
class ParticleSystem : public Positionable3d {
public:
	ParticleSystem(int size);
	~ParticleSystem();

	const std::vector<Particle*>& getParticles() const;
	void setParticleBehavior(std::function<void(Particle*, int)> handler);
	void setParticleColor(const Color& color);
	void setParticleSize(float width, float height);
	void setParticleTexture(TextureBuffer* texture);
	void setSpawnLocation(const Vec3& location);
	void setSpawnRange(const Range<float> xSpawnRange, const Range<float> ySpawnRange, const Range<float> zSpawnRange);
	void update(int dt);

private:
	std::vector<Particle*> particles;
	std::function<void(Particle*, int)> behaviorHandler;
	Range<float> xSpawnRange = { 0.0f, 0.0f };
	Range<float> ySpawnRange = { 0.0f, 0.0f };
	Range<float> zSpawnRange = { 0.0f, 0.0f };

	void resetParticle(Particle* particle);
};
