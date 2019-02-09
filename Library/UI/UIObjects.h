#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

struct UIObject {
	~UIObject();

	void setPosition(int x, int y);
	void setRenderer(SDL_Renderer* renderer);
	virtual void draw() = 0;

protected:
	SDL_Renderer* m_renderer = 0;
	SDL_Texture* m_texture = 0;
	SDL_Rect m_rect;

	virtual void refresh() = 0;
	void setTextureFromSurface(SDL_Surface* surface);
};

struct UIText : UIObject {
	UIText(const char* value);

	void draw();
	void setFont(TTF_Font* font);
	void setColor(const SDL_Color &color);
	void setValue(const char* value);

private:
	const char* m_value = 0;
	TTF_Font* m_font = 0;
	SDL_Color m_color = { 255, 255, 255 };

	void refresh();
};
