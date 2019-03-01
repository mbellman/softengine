#include <Graphics/TextureBuffer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <Graphics/Color.h>
#include <Graphics/ColorBuffer.h>
#include <Helpers.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

static int getTotalMipmaps(int size) {
	return size <=2 ? 0 : 1 + getTotalMipmaps(size >> 1);
}

static Uint32 readPixel(SDL_Surface* surface, int index) {
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

	return ARGB(R, G, B);
}

/**
 * TextureBuffer
 * -------------
 */
TextureBuffer::TextureBuffer(const char* file) {
	this->file = file;
}

TextureBuffer::~TextureBuffer() {
	if (mipmaps.size() > 0) {
		// Free software texture memory
		for (const auto* mipmap : mipmaps) {
			delete mipmap;
		}

		mipmaps.clear();
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
			ColorBuffer* colorBuffer = new ColorBuffer(width, height);

			for (int i = 0; i < totalPixels; i++) {
				Uint32 color = readPixel(image, i);
				int x = i % width;
				int y = (int)(i / width);

				colorBuffer->write(x, y, (color & 0x00FF0000) >> 16, (color & 0x0000FF00) >> 8, color & 0x000000FF);
			}

			mipmaps.push_back(colorBuffer);

			if (shouldUseMipmaps) {
				int totalMipMaps = getTotalMipmaps(std::min(width, height));
				ColorBuffer* mipmap = colorBuffer;

				for (int i = 0; i < totalMipMaps; i++) {
					mipmap = mipmap->createDownsizedBuffer();

					mipmaps.push_back(mipmap);
				}
			}
		}

		SDL_FreeSurface(image);
	}
}

const ColorBuffer* TextureBuffer::getMipmap(int level) const {
	return level >= mipmaps.size() ? mipmaps.back() : mipmaps.at(level);
}

const Color& TextureBuffer::sample(float u, float v, const ColorBuffer* mipmap) const {
	if (mipmaps.empty()) {
		return BLACK;
	}

	// Modulo-free out-of-bounds UV wrapping
	if (u >= 1.0f) u -= (int)u;
	else if (u < 0.0f) u += (int)(-1.0f * (u - 1.0f));

	if (v >= 1.0f) v -= (int)v;
	else if (v < 0.0f) v += (int)(-1.0f * (v - 1.0f));

	int pixelIndex = (int)(v * (mipmap->height)) * mipmap->width + (int)(u * mipmap->width);

	return mipmap->read(pixelIndex);
}
