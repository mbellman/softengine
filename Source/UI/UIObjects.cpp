#include <UI/UIObjects.h>

void UIObject::setPosition(int x, int y) {
	m_rect.x = x;
	m_rect.y = y;
}

void UIText::setValue(const char* value) {
	m_surface = TTF_RenderText_Solid(m_font, value, m_color);
	m_value = value;
	TTF_SizeText(m_font, m_value, &m_rect.w, &m_rect.h);
}

void UIText::setColor(const SDL_Color &color) {
	m_color = color;
}

void UIText::setFont(TTF_Font* font) {
	m_font = font;
}

void UIText::draw(SDL_Renderer* renderer) {
	m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
	SDL_RenderCopy(renderer, m_texture, NULL, &m_rect);
}

