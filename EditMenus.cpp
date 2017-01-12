#include "EditMenus.h"

#include "SDL_include.h"
using namespace widgets;

EditMenus::EditMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool& mini)
{
	selected_id = 0;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	texture = newtexture;
	SDL_Color color;
	color.a = 255;
	color.b = 0;
	color.g = 0;
	color.r = 0;
	visible = false;

	std::string xmlpath = "assets//xml//edit_menu_config.xml";
	std::string xmltype = "edit";
	hxmlFile = new rapidxml::file<>(xmlpath.c_str()); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node(xmltype.c_str());
	}
	else{
		pNode = hpRoot->first_node("config")->first_node(xmltype.c_str());
	}
	sNode = pNode->first_node("button_displacement");
	for (int i = 0; i < 3; ++i){
		for (int j = 0; j < 3; ++j){
			slots.push_back(MeguButton(rect.x + atoi(sNode->first_node("pos")->first_node("x")->value()) + i*atoi(sNode->first_node("offset")->first_node("x")->value()),
				rect.y + atoi(sNode->first_node("pos")->first_node("y")->value()) + j*atoi(sNode->first_node("offset")->first_node("y")->value()),
				atoi(sNode->first_node("pos")->first_node("w")->value()),
				atoi(sNode->first_node("pos")->first_node("h")->value()), NULL, false));
		}
	}
	
	controls.push_back(MeguButton(rect.x + atoi(pNode->first_node("back_button")->first_node("pos")->first_node("x")->value())
		, rect.y + atoi(pNode->first_node("back_button")->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("back_button")->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("back_button")->first_node("pos")->first_node("h")->value()), NULL, false));
	
	for (widgets::button_container::iterator it = slots.begin(); it != slots.end(); ++it){
		it->setEnable(false);
		it->setVisible(false);
	}
	for (std::vector<TtfWrapper>::iterator it = slots_name.begin(); it != slots_name.end(); ++it)
		it->setVisible(false);

	controls[0].setEnable(false);
	controls[0].setVisible(false);
}

EditMenus::~EditMenus()
{
}

SDL_Texture* EditMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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

