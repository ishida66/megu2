#pragma once
#include "TtfWrapper.h"
#include "SDL_include.h"
#include <string>
class TextEdit : public TtfWrapper
{
public:
	SDL_Rect click_area;
	//Initializes variables
	TextEdit(){}
	TextEdit(const int &x, const int &y, const int &w, const int &h,
		const Uint8 &red, const Uint8 &green, const Uint8 &blue,
		const SDL_BlendMode &blending, const Uint8 &alpha,
		const SDL_Color &_textColor, const std::string &font_name, const int &font_size,const bool &_visible){
		click_area.x = x;
		click_area.y = y;
		click_area.w = w;
		click_area.h = h;
		renderQuad.x = x;
		renderQuad.y = y;
		textColor = _textColor;
		setColor(red, green, blue);
		setBlendMode(blending);
		setAlpha(alpha);
		font = TTF_OpenFont(font_name.c_str(), font_size);
		visible = _visible;
		
	}
	~TextEdit(){}

	bool isIn(const int &x, const int &y){
		if (click_area.x <= x && x <= click_area.x + click_area.w  && click_area.y <= y  && y <= click_area.y + click_area.h){
			return true;
		}
		return false;
	}

	void handleKey(SDL_Event *e){
		//Special key input
					if (e->type == SDL_KEYDOWN)
					{
						//Handle backspace
						if (e->key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)
						{
							//lop off character
							text.pop_back();
						}
						//Handle copy
						else if (e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
						{
							text = SDL_SetClipboardText(text.c_str());
						}
						//Handle paste
						else if (e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
						{
							text = SDL_GetClipboardText();
						}
						if (e->key.keysym.sym == SDLK_RETURN)
							setFocus(false);
					}
				//Special text input event
						else if (e->type == SDL_TEXTINPUT)
					{
						//Not copy or pasting
						if (!((e->text.text[0] == 'c' || e->text.text[0] == 'C') && (e->text.text[0] == 'v' || e->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
						{
							//Append character
							text += e->text.text;
						}
					}
	}
	bool getFocus(){
		return focus;
	}
	void setFocus(bool _focus){
		focus = _focus;
	}
private:
	bool focus;
};