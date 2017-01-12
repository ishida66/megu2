#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>

namespace widgets{
	class EditMenus :
		public MeguMenus
	{
		typedef int ID;
	public:
		EditMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void initBackButton(SDL_Texture *b_texture);
		~EditMenus();
		MeguButton *back_button;
	protected:
		
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		
	};
}
