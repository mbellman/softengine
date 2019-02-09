#include <Graphics/TextureBuffer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <Graphics/Color.h>
#include <Helpers.h>

/**
 * TextureBuffer
 * -------------
 */
TextureBuffer::TextureBuffer(const char* file) {
	this->file = file;
}

TextureBuffer::~TextureBuffer() {
	if (pixels != NULL) {
		// Free software texture memory
		delete[] pixels;
	} else if (texture != NULL) {
		// Free hardware texture memory
		SDL_DestroyTexture(texture);
	}
}

void TextureBuffer::confirmTexture(SDL_Renderer* renderer, TextureMode mode) {
	if (!isConfirmed) {
		isConfirmed = true;

		SDL_Surface* image = IMG_Load(file);

		if (image == NULL) {
			return;
		}

		// Cache texture width and height information so
		// UV coordinates can be mapped to a pixel index
		width = image->w;
		height = image->h;
		totalPixels = image->w * image->h;

		if (mode == TextureMode::HARDWARE) {
			texture = SDL_CreateTextureFromSurface(renderer, image);
		} else if (mode == TextureMode::SOFTWARE) {
			SDL_PixelFormat* format = image->format;
			pixels = new Color[totalPixels];

			for (int i = 0; i < totalPixels; i++) {
				savePixel(image, i);
			}
		}

		SDL_FreeSurface(image);
	}
}

const Color& TextureBuffer::sample(float u, float v) const {
	if (pixels == NULL) {
		return BLACK;
	}

	int index = (int)(v * height) * width + (int)(u * width);

	return index >= 0 && index < totalPixels ? pixels[index] : BLACK;
}

void TextureBuffer::savePixel(SDL_Surface* surface, int index) {
	int x = index % surface->w;
	int y = (int)(index / surface->w);
	SDL_PixelFormat* format = surface->format;
	Uint8* pixel = (Uint8*)surface->pixels + surface->pitch * y + format->BytesPerPixel * x;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	int R = pixel[0];
	int G = pixel[1];
	int B = pixel[2];
#else
	int R = pixel[format->BytesPerPixel - 1];
	int G = pixel[format->BytesPerPixel - 2];
	int B = pixel[format->BytesPerPixel - 3];
#endif

	pixels[index] = { R, G, B };
}
