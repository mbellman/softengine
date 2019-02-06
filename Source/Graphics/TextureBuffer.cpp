#include <SDL.h>
#include <SDL_image.h>
#include <Graphics/TextureBuffer.h>
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

const Uint32* TextureBuffer::getSoftwareTexture() {
	return pixels;
}

const SDL_Texture* TextureBuffer::getHardwareTexture() {
	return texture;
}

void TextureBuffer::confirmTexture(SDL_Renderer* renderer, TextureMode mode) {
	if (!isConfirmed) {
		isConfirmed = true;

		SDL_Surface* image = IMG_Load(file);

		if (image == NULL) {
			return;
		}

		if (mode == TextureMode::HARDWARE) {
			texture = SDL_CreateTextureFromSurface(renderer, image);
		} else if (mode == TextureMode::SOFTWARE) {
			// In software mode, we have to convert the loaded image to
			// a pixel data buffer compatible with the Rasterizer pixel
			// buffer (Uint32 ARGB).
			int totalPixels = image->w * image->h;
			SDL_PixelFormat* format = image->format;
			pixels = new Uint32[totalPixels];

			for (int i = 0; i < totalPixels; i++) {
				savePixel(image, i);
			}
		}

		SDL_FreeSurface(image);
	}
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

	pixels[index] = ARGB(R, G, B);
}
