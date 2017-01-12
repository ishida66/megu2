#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <map>
#include "TextEdit.h"
namespace widgets{
	class SizeMenus
	{
		typedef int ID;
	public:
		SizeMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer);
		~SizeMenus();
		void disableMenu();
		void callMenu();
		void Render(SDL_Renderer *renderer);
		std::vector<MeguButton>  buttons;
	protected:
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		bool visible;
		SDL_Rect rect;
		SDL_Texture *texture;
		rapidxml::file<> *hxmlFile;
		rapidxml::xml_document<> hdoc;
		rapidxml::xml_node<> *hpRoot;
		rapidxml::xml_node<> *pNode;
		rapidxml::xml_node<> *sNode;
		rapidxml::xml_node<> *dNode;
		rapidxml::xml_node<> *nNode;
		rapidxml::xml_node<> *overlayNode;

	};
}
