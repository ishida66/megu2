#pragma once

#include<vector>
#include<map>
#include<iostream>
#include "SDL_include.h"

#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include<rapidXML/rapidxml.hpp>
#include<rapidXML/rapidxml_utils.hpp>

typedef int ID;
typedef std::string ITEM_TYPE;
class item_library{

	
	
	inline SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer)
	{
		//The final texture
		SDL_Texture* newTexture = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		}
		else
		{
			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		return newTexture;
	}
public:
	struct megu{
		ID id;
		std::string en_name;
		std::string en_detail;
		std::string en_family;
		std::wstring jp_name;
		std::wstring jp_detail;
		std::wstring jp_family;
		int size;
		int hungry;
		int change;
		float min_weight;
		float max_weight;
		SDL_Texture *front_texture;//front small image
	};
	struct item{
		ID id;

		std::string en_name;
		std::wstring jp_name;
		std::string en_detail;
		std::wstring jp_detail;

		ITEM_TYPE type;
		int nutrition;
		int calories;		
		int freshness;
		int lvl;
		int exp;
		int price;
		int animation_time;
		bool isanimated;
		std::vector<int> animation_order;
		std::vector<SDL_Texture *>texture;
	};

	struct house{
		ID id;
		std::string en_name;
		std::wstring jp_name;
		std::string en_detail;
		std::wstring jp_detail;

		int size;
		int lvl;
		int comfort;
		int price;
		SDL_Texture * front_texture;

	};

	std::map<ID, item> food_lib;
	std::map<ID, item> furniture_lib;
	std::map<ID, item> accessory_lib;
	std::map<ID, megu> megu_lib;
	std::map<ID, house> house_lib;


	void populate(SDL_Renderer *renderer){
        std::fstream f("assets/xml/house.xml");
        if(f.is_open()){
            std::cerr << "opened!" << std::endl;
        }
        else{
            std::cerr << "it is not :(" << std::endl;
        }
		f.close();
		rapidxml::file<> hxmlFile("assets//xml//house.xml"); // Default template is char
		rapidxml::xml_document<> hdoc;
		hdoc.parse<0>(hxmlFile.data());
		rapidxml::xml_node<> *hpRoot = hdoc.first_node();
		for (rapidxml::xml_node<> *pNode = hpRoot->first_node("house"); pNode; pNode = pNode->next_sibling("house"))
		{
			house h_obj;
			h_obj.id = atoi(pNode->first_node("id")->value());
			h_obj.en_name = pNode->last_node("strings")->first_node("name")->first_node()->value();
			h_obj.en_detail = pNode->last_node("strings")->first_node("detail")->first_node()->value();
			h_obj.size = atoi(pNode->first_node("size")->value());
			h_obj.lvl = atoi(pNode->first_node("level")->value());
			h_obj.comfort = atoi(pNode->first_node("comfort")->value());
			h_obj.price = atoi(pNode->first_node("price")->value());
			h_obj.front_texture = loadTexture("assets/Houses/ID" + std::to_string(h_obj.id) + ".png", renderer);
			house_lib[h_obj.id] = h_obj;
		}
		rapidxml::file<> mxmlFile("assets/xml/megu_dl.xml"); // Default template is char
		rapidxml::xml_document<> mdoc;
		mdoc.parse<0>(mxmlFile.data());
		rapidxml::xml_node<> *mpRoot = mdoc.first_node();
		for (rapidxml::xml_node<> *pNode = mpRoot->first_node("megu"); pNode; pNode = pNode->next_sibling("megu"))
		{
			megu f_obj;
			f_obj.id = atoi(pNode->first_node("id")->value());
		//	f_obj.jp_name = (pNode->first_node("strings")->first_node("name")->value());
		//	f_obj.jp_detail = pNode->first_node("strings")->first_node("detail")->value();
		//	f_obj.jp_family = pNode->first_node("strings")->first_node("family")->value();
			f_obj.en_name = pNode->last_node("strings")->first_node("name")->value();
			f_obj.en_detail = pNode->last_node("strings")->first_node("detail")->value();
			f_obj.en_family = pNode->last_node("strings")->first_node("family")->value();
			f_obj.size = atoi(pNode->first_node("size")->value());
			f_obj.hungry = atoi(pNode->first_node("hungry")->value());
			f_obj.change = atoi(pNode->first_node("change")->value());
			f_obj.min_weight = (float)(atof(pNode->first_node("min_weight")->value()));
			f_obj.max_weight = (float)(atof(pNode->first_node("max_weight")->value()));

			f_obj.front_texture = loadTexture("assets/megu/ID" + std::to_string(f_obj.id) + "_3.png",renderer);
			megu_lib[f_obj.id] = f_obj;
		}

		rapidxml::file<> fxmlFile("assets/xml/food.xml"); // Default template is char
		rapidxml::xml_document<> fdoc;
		fdoc.parse<0>(fxmlFile.data());
		rapidxml::xml_node<> *fpRoot = fdoc.first_node();
		for (rapidxml::xml_node<> *pNode = fpRoot->first_node("food"); pNode; pNode = pNode->next_sibling("food"))
		{
			item f_obj;
			f_obj.id = atoi(pNode->first_node("id")->value());
			f_obj.en_name = pNode->last_node("strings")->first_node("name")->first_node()->value();
			f_obj.en_detail = pNode->last_node("strings")->first_node("detail")->first_node()->value();
			f_obj.type = pNode->first_node("classification")->value();
			f_obj.nutrition = atoi(pNode->first_node("nutrition")->value());
			f_obj.calories = atoi(pNode->first_node("calorie")->value());
			f_obj.freshness = atoi(pNode->first_node("freshness")->value());
			f_obj.lvl = atoi(pNode->first_node("level")->value());
			f_obj.exp = atoi(pNode->first_node("exp")->value());
			f_obj.price = atoi(pNode->first_node("price")->value());
			f_obj.texture.push_back(loadTexture("assets/Food/Food_" + std::to_string(f_obj.id) + "@2x.png", renderer));
			food_lib[f_obj.id] = f_obj;
		}

		rapidxml::file<> ixmlFile("assets/xml/furniture.xml"); // Default template is char
		rapidxml::xml_document<> idoc;
		idoc.parse<0>(ixmlFile.data());
		rapidxml::xml_node<> *ipRoot = idoc.first_node();
		for (rapidxml::xml_node<> *pNode = ipRoot->first_node("furniture"); pNode; pNode = pNode->next_sibling("furniture"))
		{
			item fur_obj;
			fur_obj.id = atoi(pNode->first_node("id")->value());
			fur_obj.en_name = pNode->last_node("strings")->first_node("name")->first_node()->value();
			fur_obj.en_detail = pNode->last_node("strings")->first_node("detail")->first_node()->value();
			fur_obj.type = "furniture";
			fur_obj.lvl = atoi(pNode->first_node("level")->value());
			fur_obj.price = atoi(pNode->first_node("price")->value());
			if (pNode->first_node("animation") != NULL){
				fur_obj.isanimated = true;
				fur_obj.animation_time = (int)(atof(pNode->first_node("animation")->first_node("duration")->value()) * 100);
				std::string buf = pNode->first_node("animation")->first_node("list")->value();
				std::stringstream ss(buf);
				int i;
				while (ss >> i)
				{
					fur_obj.animation_order.push_back(i);
					if(ss.peek() == ',' || ss.peek() == ' ')
						ss.ignore();
				}
			}
			fur_obj.texture.push_back(loadTexture("assets/Furniture/KG" + std::to_string(fur_obj.id) + "@2x.png", renderer));

			furniture_lib[fur_obj.id] = fur_obj;
		}

		rapidxml::file<> axmlFile("assets/xml/accessory.xml"); // Default template is char
		rapidxml::xml_document<> adoc;
		adoc.parse<0>(axmlFile.data());
		rapidxml::xml_node<> *apRoot = adoc.first_node();
		for (rapidxml::xml_node<> *pNode = apRoot->first_node("accessory"); pNode; pNode = pNode->next_sibling("accessory"))
		{
			item acc_obj;
			acc_obj.id = atoi(pNode->first_node("id")->value());
			acc_obj.en_name = pNode->last_node("strings")->first_node("name")->first_node()->value();
			acc_obj.en_detail = pNode->last_node("strings")->first_node("detail")->first_node()->value();
			acc_obj.type = "accessory";
			acc_obj.lvl = atoi(pNode->first_node("level")->value());
			acc_obj.price = atoi(pNode->first_node("price")->value());

			acc_obj.texture.push_back(loadTexture("assets/Accessory/FUKU" + std::to_string(acc_obj.id) + ".png", renderer));
			accessory_lib[acc_obj.id] = acc_obj;
		}
		
	}
};