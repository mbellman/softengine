#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <System/Positionable.h>
#include <Graphics/Color.h>

/**
 * UIObject
 * --------
 */
class UIObject : public Positionable2d {
public:
	~UIObject();

	void setAlpha(float alpha);
	void setPosition(int x, int y);
	void setRenderer(SDL_Renderer* renderer);
	void update(int dt);

protected:
	SDL_Renderer* m_renderer = 0;
	SDL_Texture* m_texture = 0;
	SDL_Rect m_rect;

	virtual void refresh() = 0;
	void refreshAlpha();
	void setTextureFromSurface(SDL_Surface* surface);

private:
	float alpha = 1.0f;

	Uint8 getAlphaMod();
};

/**
 * UIRect
 * ------
 */
class UIRect : public UIObject {
public:
	void setColor(const Color& color);
	void setSize(int w, int h);

protected:
	void refresh();

private:
	int width = 0;
	int height = 0;
	Color color;
};

/**
 * UIText
 * ------
 */
class UIText : public UIObject {
public:
	UIText();
	UIText(const char* value);

	void setFont(TTF_Font* font);
	void setColor(const SDL_Color &color);
	void setValue(const char* value);

protected:
	void refresh();

private:
	const char* m_value = 0;
	TTF_Font* m_font = 0;
	SDL_Color m_color = { 255, 255, 255 };
};

/**
 * UIGraphic
 * ---------
 */
class UIGraphic : public UIObject {
public:
	UIGraphic(const char* filename);

	void clip(int w, int h);
	void unclip();

protected:
	void refresh();

private:
	SDL_Surface* image = NULL;
	int width;
	int height;

	void setTransparentPixels();
};
