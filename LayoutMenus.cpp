#include "LayoutMenus.h"
#include "SDL_include.h"
#include <sstream>
#include <iomanip>
using namespace widgets;

LayoutMenus::LayoutMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini)
{
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	texture = newtexture;
	visible = false;
	SDL_Color color;
	color.a = 255;
	color.b = 0;
	color.g = 0;
	color.r = 0;
	std::string xmlpath = "assets//xml//layout_menu_config.xml";
	std::string xmltype = "layout";
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
	current_page = 0;
	//Load image at specified path
}

void LayoutMenus::RenderButtons(SDL_Renderer *renderer)
{
	if (visible){
		if (marker_visible){
			SDL_RenderCopy(renderer, marker, NULL, &marker_rect);
		}
	}
}

void LayoutMenus::setViewPage(const int &page, SDL_Renderer *renderer){
	if (save){
		if (page >= 0){
			current_page = page;
			updateSlots(renderer);
		}
	}
	else{
		std::cerr << "layout list size: " << layout_list.size() << std::endl;
		std::cerr << "page: " << page << std::endl;
		std::cerr << "page limit: " << (layout_list.size() - 1) / 3 << std::endl;
		if (page <= (layout_list.size() - 1) / 3 && page >= 0){
			
			current_page = page;
			updateSlots(renderer);
		}

	}
}

LayoutMenus::~LayoutMenus()
{
}
void LayoutMenus::parseFiles(const std::vector<std::string> &layout_friend_list){
	layout_list.clear();
	int i = 1;

	for (std::vector<std::string>::const_iterator it = layout_friend_list.begin(); it != layout_friend_list.end(); ++it){
		std::ifstream file;
		std::string filename = "gamedata/house/";
		file_data f;
		std::stringstream ss;
		std::string s;
		std::cerr << *it << std::endl;
		ss << std::setw(3) << std::setfill('0') << i;
		s= "SAV" + ss.str() + ".house";
		ss.clear();
 		while (s != *it){		
			std::cerr << "s "  << s << " " << *it << std::endl;
			f.enabled = false;
			layout_list.push_back(f);
			i= i+1;
			std::stringstream ss1;
			ss1.clear();
			ss1 << std::setw(3) << std::setfill('0') << i;
			s.clear();
			s = "SAV" + ss1.str() + ".house";
		}
		file.open(filename + *it);
				
		if (file.is_open()){
			f.path = filename;
			file >> f.img_path;
			f.enabled = true;
			layout_list.push_back(f);
			file.close();
		}
		++i;
	}
}

void LayoutMenus::updateSlots(SDL_Renderer *renderer){
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
		SDL_Texture * texture = NULL;
		if (curr_index < layout_list.size()){
			int w, h;
			texture = loadTexture("gamedata/house/preview/" + layout_list.at(curr_index).img_path, renderer);
			std::cerr << "layout list: " << layout_list.at(curr_index).img_path << std::endl;
			std::cerr << layout_list.at(curr_index).img_path << std::endl;
		}

		std::cerr << "current index: " << curr_index << std::endl;

		slots[i].changeDisplayObject(texture, marker_rect.w, marker_rect.h, curr_index + 1);
		if (save){
			slots[i].setEnable(true);
		}
		else{
			if (layout_list.at(curr_index).enabled){
				slots[i].setEnable(true);
				
			}
			else{
				slots[i].setEnable(false);
			}
		}
	}
}


SDL_Texture* LayoutMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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
void LayoutMenus::setMode(const bool &mode){
	save = mode;
}