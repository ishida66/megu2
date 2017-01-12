#pragma once
#include "SDL_include.h"
#include <string>
#include <iostream>
class TtfWrapper
{
public:
	std::string text;//written text
	//Initializes variables
	TtfWrapper(){
		//Initialize
		mTexture = NULL;
		font = TTF_OpenFont("assets/fonts/ahronbd.ttf", 28);
	}

	//Initializes variables with position
	TtfWrapper(const int &x, const int &y, 
		const Uint8 &red, const Uint8 &green, const Uint8 &blue,
		const SDL_BlendMode &blending, const Uint8 &alpha,
		const SDL_Color &_textColor, const Uint8 &_size, const std::string &fontpath, const int &letters, bool _visible){
		//Initialize
		mTexture = NULL;
		renderQuad.x = x;
		renderQuad.y = y;

		textColor = _textColor;
		setColor(red, green, blue);
		setBlendMode(blending);
		setAlpha(alpha);
		font = TTF_OpenFont(fontpath.c_str(), _size);
		visible = _visible;
		max_letters = letters;
	}

	//Deallocates memory
	~TtfWrapper(){
		//Deallocate
		free();
	}
	void setText(const std::string &_text){
		text = _text;
	}
	std::string getText(){
		return text;
	}
	//Creates image from font string
	bool loadFromRenderedText(SDL_Renderer *renderer){
		//Get rid of preexisting texture
		if (mTexture != NULL)
			free();

		//Render text surface
		
		SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, max_letters);
		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			if (mTexture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				renderQuad.w = textSurface->w;
				renderQuad.h = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}

		//Return success
		return mTexture != NULL;
	}

	//Deallocates texture
	void free(){
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
		}
	}

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue){
		//Modulate texture rgb
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	}

	//Set blending
	void setBlendMode(SDL_BlendMode blending){
		//Set blending function
		SDL_SetTextureBlendMode(mTexture, blending);
	}

	//Set alpha modulation
	void setAlpha(Uint8 alpha){
		//Modulate texture alpha
		SDL_SetTextureAlphaMod(mTexture, alpha);
	}

	//Renders texture at given point
	void Render(SDL_Renderer *renderer, SDL_RendererFlip flip = SDL_FLIP_NONE){
		//Set rendering space and render to screen
		//Render to screen
		SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
	}

	//Gets image dimensions
	int getWidth(){
		return renderQuad.w;
	}

	int getHeight(){
		return renderQuad.h;
	}

	//Visibility
	void setVisible(bool _v){
		visible = _v;
	}
	bool getVisible(){
		return visible;
	}
	
protected:
	SDL_Rect renderQuad;
	TTF_Font *font;
	//The actual hardware texture
	SDL_Texture* mTexture;
	//Image dimensions
	bool visible;
	SDL_Color textColor;
	int max_letters;
};
