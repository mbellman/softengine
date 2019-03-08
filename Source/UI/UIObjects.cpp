#include <UI/UIObjects.h>
#include <Graphics/TextureBuffer.h>
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

Uint8 UIObject::getAlphaMod() {
	return alpha * 255;
}

void UIObject::setAlpha(float alpha) {
	this->alpha = alpha;
	refresh();
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

void UIObject::update(int dt) {
	updatePosition(dt);

	if (m_texture != NULL) {
		m_rect.x = position.x;
		m_rect.y = position.y;

		SDL_RenderCopy(m_renderer, m_texture, NULL, &m_rect);
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

	m_rect.w = w;
	m_rect.h = h;

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

		TTF_SizeText(m_font, m_value, &m_rect.w, &m_rect.h);
		setTextureFromSurface(m_surface);
		refreshAlpha();
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
	width = image->w;
	height = image->h;

	unclip();
	setTransparentPixels();
}

void UIGraphic::clip(int w, int h) {
	m_rect.w = w;
	m_rect.h = h;
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

void UIGraphic::unclip() {
	m_rect.w = width;
	m_rect.h = height;
}
