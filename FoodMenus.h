#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include"Items_library.h"
#include "SDL_include.h"

#include <map>
namespace widgets{
	class FoodMenus :
		public MeguMenus
	{
		typedef int ID;
	public:
		FoodMenus(const int &x, const int &y, const  int &width, const  int &height, std::map<ID, item_library::item> *_library, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		~FoodMenus();
		void updateSlots(SDL_Renderer *renderer);
		std::map<std::string, MeguButton*> display_selector;
		std::string isInDisplay(int x, int y);
		SDL_Texture *marker;
		SDL_Rect marker_rect;
		bool marker_visible;
	protected:
		std::map<ID, item_library::item> *library;
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		

	};
}
