#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>
namespace widgets{
	class LayoutMenus :
		public MeguMenus
	{
		struct file_data{
			std::string path;
			std::string img_path;//path to preview png
			bool enabled;
		};
		typedef int ID;
	public:
		LayoutMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void updateSlots(SDL_Renderer *renderer);
		void parseFiles(const std::vector<std::string> &layout_file_list);
		void setMode(const bool &mode);
		void setViewPage(const int &page, SDL_Renderer *renderer);
		~LayoutMenus();
		std::string isInDisplay(int x, int y);
		SDL_Texture *marker;
		SDL_Rect marker_rect;
		bool marker_visible;
		bool save;//true yes, false no
	protected:
		std::vector<file_data> layout_list;
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);


	};
}
