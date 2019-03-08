#include <UI/UIObjects.h>
#include <Graphics/TextureBuffer.h>
#include <UI/Alert.h>
#include <Constants.h>
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>

/**
 * UIObject
 * --------
 */
UIObject::~UIObject() {
	if (m_texture != NULL) {
		SDL_DestroyTexture(m_texture);
	}
}

void UIObject::clip(int w, int h) {
	sourceRect.w = w;
	sourceRect.h = h;

	destRect.w = w;
	destRect.h = h;
}

Uint8 UIObject::getAlphaMod() {
	return alpha * 255;
}

void UIObject::setAlpha(float alpha) {
	this->alpha = alpha;

	if (m_texture != NULL) {
		refreshAlpha();
	}
}

void UIObject::refreshAlpha() {
	SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(m_texture, getAlphaMod());
}

void UIObject::setRenderer(SDL_Renderer* renderer) {
	m_renderer = renderer;

	refresh();
}

void UIObject::setTextureFromSurface(SDL_Surface* surface) {
	m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_FreeSurface(surface);
}

void UIObject::unclip() {
	sourceRect.w = width;
	sourceRect.h = height;

	destRect.w = width;
	destRect.h = height;
}

void UIObject::update(int dt) {
	updatePosition(dt);

	if (m_texture != NULL) {
		destRect.x = position.x;
		destRect.y = position.y;

		SDL_RenderCopy(m_renderer, m_texture, &sourceRect, &destRect);
	}
}

/**
 * UIRect
 * ------
 */
void UIRect::refresh() {
	if (m_renderer != NULL && width > 0 && height > 0) {
		if (m_texture != NULL) {
			SDL_DestroyTexture(m_texture);
		}

		Uint32* pixels = new Uint32[width * height];
		Uint32 fillColor = ARGB(color.R, color.G, color.B);
		m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

		std::fill(pixels, pixels + width * height, fillColor);
		SDL_UpdateTexture(m_texture, NULL, pixels, width * sizeof(Uint32));
		refreshAlpha();

		delete[] pixels;
	}
}

void UIRect::setColor(const Color& color) {
	this->color = color;

	refresh();
}

void UIRect::setSize(int w, int h) {
	width = w;
	height = h;

	unclip();
	refresh();
}

/**
 * UIText
 * ------
 */
UIText::UIText() {}

UIText::UIText(const char* value) {
	m_value = value;
}

void UIText::refresh() {
	if (m_renderer != NULL && m_font != NULL && m_value != NULL) {
		if (m_texture != NULL) {
			SDL_DestroyTexture(m_texture);
		}

		SDL_Surface* m_surface = TTF_RenderText_Solid(m_font, m_value, m_color);

		TTF_SizeText(m_font, m_value, &width, &height);
		setTextureFromSurface(m_surface);
		refreshAlpha();
		unclip();
	}
}

void UIText::setColor(const SDL_Color &color) {
	m_color = color;
	refresh();
}

void UIText::setFont(TTF_Font* font) {
	m_font = font;
	refresh();
}

void UIText::setValue(const char* value) {
	m_value = value;
	refresh();
}

/**
 * UIGraphic
 * ---------
 */
UIGraphic::UIGraphic(const char* filename) {
	image = IMG_Load(filename);

	if (!image) {
		char errorMessage[60];

		sprintf(errorMessage, "Unable to load image: %s", filename);
		Alert::error(ALERT_ASSET_ERROR, errorMessage);
		exit(0);
	}

	width = image->w;
	height = image->h;

	unclip();
	setTransparentPixels();
}

void UIGraphic::refresh() {
	if (image != NULL) {
		setTextureFromSurface(image);
		refreshAlpha();

		// We can safely reset the pointer since
		// the surface data is now freed
		image = NULL;
	}
}

void UIGraphic::setTransparentPixels() {
	if (image == NULL) {
		return;
	}

	SDL_PixelFormat* format = image->format;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int pixelIndex = y * width + x;
			Uint32 color = TextureBuffer::readPixel(image, pixelIndex);

			int R = (color & 0x00FF0000) >> 16;
			int G = (color & 0x0000FF00) >> 8;
			int B = (color & 0x000000FF);

			if (R == COLOR_TRANSPARENT.R && G == COLOR_TRANSPARENT.G && B == COLOR_TRANSPARENT.B) {
				Uint8* pixel = (Uint8*)image->pixels + image->pitch * y + format->BytesPerPixel * x;

			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				pixel[3] = 0;
			#else
				pixel[0] = 0;
			#endif
			}
		}
	}
}
