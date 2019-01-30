#include <UI/UIObjects.h>

UIObject::~UIObject() {
	if (m_texture != NULL) {
		SDL_DestroyTexture(m_texture);
	}
}

void UIObject::setPosition(int x, int y) {
	m_rect.x = x;
	m_rect.y = y;
}

void UIObject::setRenderer(SDL_Renderer* renderer) {
	this->m_renderer = renderer;
	refresh();
}

void UIObject::setTextureFromSurface(SDL_Surface* surface) {
	m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_FreeSurface(surface);
}

UIText::UIText(const char* value) {
	m_value = value;
}

void UIText::setValue(const char* value) {
	m_value = value;
	refresh();
}

void UIText::setColor(const SDL_Color &color) {
	m_color = color;
	refresh();
}

void UIText::setFont(TTF_Font* font) {
	m_font = font;
	refresh();
}

void UIText::refresh() {
	if (m_renderer != NULL && m_font != NULL && m_value != NULL) {
		if (m_texture != NULL) {
			SDL_DestroyTexture(m_texture);
		}

		SDL_Surface* m_surface = TTF_RenderText_Solid(m_font, m_value, m_color);

		TTF_SizeText(m_font, m_value, &m_rect.w, &m_rect.h);
		setTextureFromSurface(m_surface);
	}
}

void UIText::draw() {
	if (m_texture != NULL) {
		SDL_RenderCopy(m_renderer, m_texture, NULL, &m_rect);
	}
}
