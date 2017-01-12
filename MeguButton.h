#pragma once
#include "SDL_include.h"
#include <string>
#include <iostream>

namespace widgets{
	class MeguButton
	{
	private:
		SDL_Texture* texture;
		SDL_Rect rect;
		SDL_Rect obj_rect;
		//Load image at specified path
		bool visible;//if yes then render/do stuff
		bool enabled;//if yes then active/otherwise no
		int id; // id for object, if applicable



	public:
		MeguButton(){
		};

		MeguButton(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, const bool &_visible = true)
		{
			texture = NULL;
			rect.x = x;
			rect.y = y;
			obj_rect.x = x;
			obj_rect.y = y;
			rect.w = width;
			rect.h = height;
			obj_rect.w = width;
			obj_rect.h = height;
			texture = newtexture;
			visible = _visible;
			enabled = _visible;
		}
		int getX(){
			return rect.x;
		}
		int getY(){
			return rect.y;
		}
		void changeObject(SDL_Texture* newtexture){
			texture = NULL;
			texture = newtexture;
		}

		void changeDisplayObject(SDL_Texture* newtexture, const int &w, const int &h, const int &_id){
			texture = newtexture;
			obj_rect.w = w;
			obj_rect.h = h;
			obj_rect.x = rect.x + (rect.w- obj_rect.w)/2;
			obj_rect.y = rect.y + (rect.h- obj_rect.h)/2;
			id = _id;
		}

		void changePos(const int &x, const int &y){
			rect.x = x;
			rect.y = y;
		}
		void setVisible(bool _visible){
			visible = _visible;
		}
		void setEnable(bool _enabled){
			enabled = _enabled;
		}
		bool getEnable(){
			return enabled;
		}
		bool IsIn(int mouseX, int mouseY)
		{
			if (((mouseX > rect.x) && (mouseX < rect.x + rect.w))
				&& ((mouseY > rect.y) && (mouseY < rect.y + rect.h))
				&& enabled) {
				return true;
			}
			else {
				return false;
			}
		}

		int getId(){
			return id;
		}

		void Render(SDL_Renderer *renderer)
		{
			if (visible)
				SDL_RenderCopy(renderer, texture, NULL, &obj_rect);
			// use the same code you use now to render the button in OpenGL/SDL
		}

		~MeguButton(){
			SDL_DestroyTexture(texture);
		}
	};
}