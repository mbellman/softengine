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

	static Uint32 readPixel(SDL_Surface* surface, int index);
	void confirmTexture(SDL_Renderer* renderer, TextureMode mode);
	const ColorBuffer* getMipmap(int level) const;
	const Color& sample(float u, float v, const ColorBuffer* mipmap) const;

private:
	constexpr static Color BLACK = { 0, 0, 0 };
	bool isConfirmed = false;
	const char* file;
	std::vector<const ColorBuffer*> mipmaps;
	SDL_Texture* texture = NULL;

	void savePixel(SDL_Surface* surface, int index);
};
