#pragma once

#include <System/Objects.h>
#include <Graphics/Color.h>
#include <Graphics/TextureBuffer.h>
#include <functional>
#include <vector>

/**
 * ParticleSystem
 * --------------
 */
class ParticleSystem {
public:
	ParticleSystem(int size);
	~ParticleSystem();

	const std::vector<Particle*>& getParticles() const;
	void setBehavior(std::function<void(Particle*, int)> handler);
	void setLocation(const Vec3 location);
	void setParticleColor(const Color& color);
	void setParticleSize(float width, float height);
	void setParticleTexture(TextureBuffer* texture);
	void setRange(const Vec3 r1, const Vec3 r2);
	void update(int dt);

private:
	std::vector<Particle*> particles;
	std::function<void(Particle*, int)> behaviorHandler;
	Vec3 range[2];

	void resetParticle(Particle* particle);
};
