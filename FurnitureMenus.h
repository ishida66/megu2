#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>
namespace widgets{
	class FurnitureMenus :
		public MeguMenus
	{
		typedef int ID;
	public:
		FurnitureMenus(const int &x, const int &y, const  int &width, const  int &height, std::map<ID, item_library::item> *_library, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void updateSlots(SDL_Renderer *renderer);
		~FurnitureMenus();
		std::string isInDisplay(int x, int y);
		MeguButton *goright;
		MeguButton *goleft;
		SDL_Texture *marker;
		SDL_Rect marker_rect;
		bool marker_visible;
		MeguButton *back_button;
		MeguButton *select_button;
	protected:
		std::map<ID, item_library::item> *library;
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		SDL_Texture *buttons_overlay;
		SDL_Rect overlay_pos;
		

	};
}
