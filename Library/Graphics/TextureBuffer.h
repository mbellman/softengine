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
	int width = 0;
	int height = 0;

	TextureBuffer(const char* file);
	~TextureBuffer();

	void confirmTexture(SDL_Renderer* renderer, TextureMode mode);
	Uint32 sample(int u, int v);

private:
	bool isConfirmed = false;
	const char* file;
	Uint32* pixels = NULL;
	SDL_Texture* texture = NULL;

	void savePixel(SDL_Surface* surface, int index);
};
