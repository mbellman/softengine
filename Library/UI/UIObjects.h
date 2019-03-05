#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <System/Positionable.h>

/**
 * UIObject
 * --------
 */
struct UIObject : public Positionable2d {
	~UIObject();

	virtual void refresh() = 0;
	void setPosition(int x, int y);
	void setRenderer(SDL_Renderer* renderer);
	void update(int dt);

protected:
	SDL_Renderer* m_renderer = 0;
	SDL_Texture* m_texture = 0;
	SDL_Rect m_rect;

	void setTextureFromSurface(SDL_Surface* surface);
};

/**
 * UIText
 * ------
 */
struct UIText : UIObject {
	UIText();
	UIText(const char* value);

	void refresh();
	void setFont(TTF_Font* font);
	void setColor(const SDL_Color &color);
	void setValue(const char* value);

private:
	const char* m_value = 0;
	TTF_Font* m_font = 0;
	SDL_Color m_color = { 255, 255, 255 };
};

/**
 * UIGraphic
 * ---------
 */
struct UIGraphic : UIObject {
	UIGraphic(const char* filename);

	void clip(int w, int h);
	void refresh();
	void unclip();

private:
	SDL_Surface* image = NULL;
	int width;
	int height;

	void setTransparentPixels();
};
