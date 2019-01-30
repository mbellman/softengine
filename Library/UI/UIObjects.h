#pragma once

#include <string>

#include <SDL.h>
#include <SDL2/SDL_ttf.h>

#include <Objects.h>

struct UIObject {
	void setPosition(int x, int y);
	virtual void draw(SDL_Renderer* renderer) = 0;

protected:
	SDL_Surface* m_surface;
	SDL_Texture* m_texture;
	SDL_Rect m_rect;
};

struct UIText : UIObject {
	void draw(SDL_Renderer* renderer);
	void setFont(TTF_Font* font);
	void setColor(const SDL_Color &color);
	void setValue(const char* value);

protected:
	const char* m_value;
	TTF_Font* m_font;
	SDL_Color m_color;
};
