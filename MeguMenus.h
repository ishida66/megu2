#pragma once
#include "SDL_include.h"
#include "MeguButton.h"
#include  "TtfWrapper.h"
#include "Items_library.h"
#include <rapidXML/rapidxml.hpp>
#include <rapidXML/rapidxml_utils.hpp>
#include <map>
#include <vector>
namespace widgets{
	typedef std::string CLASSIFICATION;
	typedef int ID;
	typedef std::vector<MeguButton> button_container;
	class MeguMenus
	{
		
	public:
		MeguMenus();
		MeguMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture);
		void disableMenu();
		void callMenu();
		void Render(SDL_Renderer *renderer);
		~MeguMenus();
		button_container slots;
		button_container controls;
		std::vector<TtfWrapper> slots_name;
		std::vector<TtfWrapper> slots_description;
		int current_page;
		CLASSIFICATION current_classification;
		std::map<CLASSIFICATION, std::vector<ID>> id_library;
		void setClassificationType(const CLASSIFICATION &classification, SDL_Renderer *renderer);
		virtual void updateSlots(SDL_Renderer *renderer);
		virtual void setViewPage(const int &page, SDL_Renderer *renderer);
		void setCurrentId(const int &id);
		int getCurrentId();

	protected:
		//Globally used font
		int selected_id;
		SDL_Texture* texture;
		SDL_Rect rect;
		int w, h;
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