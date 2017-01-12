#include "SizeMenus.h"
#include "SDL_include.h"
using namespace widgets;

SizeMenus::SizeMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer)
{
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

	std::string xmlpath = "assets//xml//size_menu_config.xml";
	std::string xmltype = "size_selector";
	hxmlFile = new rapidxml::file<>(xmlpath.c_str()); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

		pNode = hpRoot->first_node("config")->first_node("size_selector");
	for (rapidxml::xml_node<> *bNode = pNode->first_node("button"); bNode; bNode = bNode->next_sibling("button"))
	{
		buttons.push_back(MeguButton(
			rect.x + atoi(bNode->first_node("pos")->first_node("x")->value()),
			rect.y + atoi(bNode->first_node("pos")->first_node("y")->value()),
			atoi(bNode->first_node("pos")->first_node("w")->value()),
			atoi(bNode->first_node("pos")->first_node("h")->value()),
			NULL,
			false));
	}
}

SizeMenus::~SizeMenus()
{
}

void SizeMenus::disableMenu(){
	visible = false;

	for (std::vector<MeguButton>::iterator te_it = buttons.begin(); te_it != buttons.end(); ++te_it){
		te_it->setEnable(false);
		te_it->setVisible(false);
	}
}

void SizeMenus::callMenu(){
	visible = true;
	for (std::vector<MeguButton>::iterator te_it = buttons.begin(); te_it != buttons.end(); ++te_it){
		te_it->setEnable(true);
		te_it->setVisible(true);
	}
}
void SizeMenus::Render(SDL_Renderer *renderer)
{
	if (visible){
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		for (std::vector<MeguButton>::iterator te_it = buttons.begin(); te_it != buttons.end(); ++te_it){
			te_it->Render(renderer);
		}
	}
}


SDL_Texture* SizeMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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