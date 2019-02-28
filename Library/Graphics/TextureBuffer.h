#pragma once

#include <SDL.h>
#include <Graphics/Color.h>
#include <Graphics/ColorBuffer.h>
#include <vector>

/**
 * TextureMode
 * -----------
 */
enum TextureMode {
	SOFTWARE,
	HARDWARE
};

/**
 * TextureBuffer
 * -------------
 */
struct TextureBuffer {
	int width = 0;
	int height = 0;
	int totalPixels = 0;
	bool shouldUseMipmaps = true;

	TextureBuffer(const char* file);
	~TextureBuffer();

	void confirmTexture(SDL_Renderer* renderer, TextureMode mode);
	const Color& sample(float u, float v, int level) const;

private:
	constexpr static Color BLACK = { 0, 0, 0 };
	bool isConfirmed = false;
	const char* file;
	std::vector<const ColorBuffer*> mipmaps;
	SDL_Texture* texture = NULL;

	void savePixel(SDL_Surface* surface, int index);
};
