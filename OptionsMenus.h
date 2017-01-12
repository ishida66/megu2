#pragma once
#include "MeguMenus.h"
#include "MeguButton.h"
#include "SDL_include.h"
#include"Items_library.h"
#include <rapidXML/rapidxml.hpp>
#include <rapidXML/rapidxml_utils.hpp>
#include <map>
namespace widgets{
	class OptionsMenus
	{
		typedef std::vector<MeguButton> button_container;

	public:
		OptionsMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini);
		void loadSelectorTexture(const std::string &filename);
		void RenderButtons(SDL_Renderer *renderer);
		void disableMenu();
		void callMenu();
		void Render(SDL_Renderer *renderer);
		int getBGMLevel();
		int getSFXLevel();
		bool getMSGOn();
		void setBGMLevel(const int&bgm);
		void setSFXLevel(const int&sfx);
		void setMSGOn(const bool&msg);
		void updateMarkers();
		~OptionsMenus();
		button_container option_buttons;
		button_container controls;
	protected:
		SDL_Rect bgm_marker_cd;
		SDL_Rect bgm_marker_pos;
		int bgm_marker_offset;
		SDL_Rect sfx_marker_cd;
		SDL_Rect sfx_marker_pos;
		int sfx_marker_offset;
		int bgm_level;
		int sfx_level;
		bool msg_on;
		SDL_Texture* marker;
		SDL_Rect msg_on_pos;

		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		SDL_Texture* texture;
		SDL_Rect rect;
		//Load image at specified path
		bool visible;//if yes then render/do stuff
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
