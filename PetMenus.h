#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>
namespace widgets{
	class PetMenus :
		public MeguMenus
	{
		typedef int ID;
	public:
		PetMenus(const int &x, const int &y, const  int &width, const  int &height, std::map<ID, item_library::megu> *_library, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void updateSlots(SDL_Renderer *renderer);
		~PetMenus();
		std::string isInDisplay(int x, int y);
		SDL_Texture *marker;
		SDL_Rect marker_rect;
		bool marker_visible;
	protected:
		std::map<ID, item_library::megu> *library;
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		SDL_Texture *buttons_overlay;
		SDL_Rect overlay_pos;
		

	};
}
