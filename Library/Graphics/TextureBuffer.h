#pragma once

#include <SDL.h>

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
	TextureBuffer(const char* file);
	~TextureBuffer();

	void confirmTexture(SDL_Renderer* renderer, TextureMode mode);
	const Uint32* getSoftwareTexture();
	const SDL_Texture* getHardwareTexture();

private:
	bool isConfirmed = false;
	const char* file;
	Uint32* pixels = NULL;
	SDL_Texture* texture = NULL;

	void savePixel(SDL_Surface* surface, int index);
};
