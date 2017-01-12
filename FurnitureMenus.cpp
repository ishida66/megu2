#include "FurnitureMenus.h"
#include "SDL_include.h"
using namespace widgets;

FurnitureMenus::FurnitureMenus(const int &x, const int &y, const  int &width, const  int &height, std::map<ID, item_library::item> *_library, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini)
{
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	texture = newtexture;
	visible = false;
	library = _library;
	current_classification = "furniture";
	SDL_Color color;
	color.a = 255;
	color.b = 0;
	color.g = 0;
	color.r = 0;
	std::string xmlpath = "assets//xml//furniture_menu_config.xml";
	std::string xmltype = "furniture";
	hxmlFile = new rapidxml::file<>(xmlpath.c_str()); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node(xmltype.c_str());
	}
	else{
		pNode = hpRoot->first_node("config")->first_node(xmltype.c_str());
	}
	sNode = pNode->first_node("slots");
	nNode = pNode->first_node("slot_name");
	dNode = pNode->first_node("slot_description");
	w = atoi(sNode->first_node("pos")->first_node("w")->value());
	h = atoi(sNode->first_node("pos")->first_node("h")->value());
	for (int i = 0; i < 3; ++i){
		slots.push_back(MeguButton(rect.x + atoi(sNode->first_node("pos")->first_node("x")->value()),
			rect.y + i * atoi(sNode->first_node("offset")->first_node("y")->value()) + atoi(sNode->first_node("pos")->first_node("y")->value()),
			atoi(sNode->first_node("pos")->first_node("w")->value()),
			atoi(sNode->first_node("pos")->first_node("h")->value()), NULL, false));

		slots_name.push_back(TtfWrapper(rect.x + atoi(nNode->first_node("pos")->first_node("x")->value())
			, rect.y + i * atoi(nNode->first_node("offset")->first_node("y")->value()) + atoi(nNode->first_node("pos")->first_node("y")->value())
			, 255, 255, 255, SDL_BLENDMODE_BLEND, 0, color
			, atoi(nNode->first_node("font_size")->value())
			, nNode->first_node("font")->value()
			, 300
			, false)
			);


		slots_description.push_back(TtfWrapper(rect.x + atoi(dNode->first_node("pos")->first_node("x")->value())
			, rect.y + i * atoi(dNode->first_node("offset")->first_node("y")->value()) + atoi(dNode->first_node("pos")->first_node("y")->value())
			, 255, 255, 255, SDL_BLENDMODE_BLEND, 0, color
			, atoi(dNode->first_node("font_size")->value())
			, nNode->first_node("font")->value()
			, 300
			, false)
			);

	}
	marker = loadTexture(pNode->first_node("marker")->first_node("texture")->value(), renderer);
	marker_rect.w = atoi(pNode->first_node("marker")->first_node("size")->first_node("w")->value());
	marker_rect.h = atoi(pNode->first_node("marker")->first_node("size")->first_node("h")->value());
	marker_visible = false;

	for (rapidxml::xml_node<> *bNode = pNode->first_node("button"); bNode; bNode = bNode->next_sibling("button"))
	{
		controls.push_back(MeguButton(
			rect.x + atoi(bNode->first_node("pos")->first_node("x")->value())
			, rect.y + atoi(bNode->first_node("pos")->first_node("y")->value())
			, atoi(bNode->first_node("pos")->first_node("w")->value())
			, atoi(bNode->first_node("pos")->first_node("h")->value())
			, NULL
			, true)
			);
	}
	int index = 0;
	for (rapidxml::xml_node<> *bNode = pNode->first_node("button"); bNode; bNode = bNode->next_sibling("button")){
		controls[index].changeObject(loadTexture(bNode->first_node("texture")->value(), renderer));
		++index;
	}
	current_page = 0;
	//Load image at specified path
}

void FurnitureMenus::RenderButtons(SDL_Renderer *renderer)
{
	if (visible){
		if (marker_visible){
			SDL_RenderCopy(renderer, marker, NULL, &marker_rect);

		}
	}
}

void FurnitureMenus::updateSlots(SDL_Renderer *renderer){
	//clean up first
	for (int i = 0; i < 3; ++i){
		slots[i].changeObject(NULL);
		slots[i].setEnable(false);
		slots_name[i].free();
		slots_name[i].setVisible(false);
		slots_description[i].free();
		slots_description[i].setVisible(false);
	}
	for (int i = 0; i < 3; ++i){
		int curr_index = current_page * 3 + i;
		if (curr_index > id_library[current_classification].size() - 1){
			return;
		}
		else{
			int id = id_library[current_classification].at(curr_index);
			int w, h;
			SDL_QueryTexture(*library->at(id).texture.begin(), NULL, NULL, &w, &h);
			if (w >  marker_rect.w){
				float ratio = (float)h / (float)w;
				h = (int)(ratio *  marker_rect.h);
				w = marker_rect.w;
			}
			if (h >  marker_rect.h){
				float ratio = (float)w / (float)h;
				w = (int)(ratio *  marker_rect.h);
				h = marker_rect.h;
			}
			slots[i].changeDisplayObject(*library->at(id).texture.begin(), w, h, id);
			slots[i].setEnable(true);
			slots_name[i].setText(library->at(id).en_name);
			slots_name[i].loadFromRenderedText(renderer);
			slots_name[i].setVisible(true);
			slots_description[i].setText(library->at(id).en_detail);
			slots_description[i].loadFromRenderedText(renderer);
			slots_description[i].setVisible(true);
		}
	}
}
FurnitureMenus::~FurnitureMenus()
{
}
SDL_Texture* FurnitureMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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

