#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>
namespace widgets{
	class FriendsMenus :
		public MeguMenus
	{
		typedef int ID;
	public:
		FriendsMenus(const int &x, const int &y, const  int &width, const  int &height, std::map<ID, item_library::megu> *_library, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void updateSlots(SDL_Renderer *renderer);
		~FriendsMenus();
		std::string isInDisplay(int x, int y);
		void parseFiles(const std::vector<std::string> &friend_file_list);
		MeguButton *goright;
		MeguButton *goleft;
		SDL_Texture *marker;
		SDL_Rect marker_rect;
		bool marker_visible;
		MeguButton *back_button;
		MeguButton *select_button;
		void setCurrentString(const int &slot);
		void setViewPage(const int &page, SDL_Renderer *renderer);
		std::string getCurrentPath();
	protected:
		std::map<ID, item_library::megu> *library;		
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		SDL_Texture *buttons_overlay;
		SDL_Rect overlay_pos;
		std::string current_owner;
		std::string current_pet_name;
		std::string current_path;
		struct file_data{
			int ID;
			std::string path;
			std::string owner_name;
			std::string megu_name;
		};
		std::vector<file_data> friend_list;


	};
}
