#include "NameMenus.h"
#include "SDL_include.h"
using namespace widgets;

NameMenus::NameMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini)
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

	std::string xmlpath = "assets//xml//name_menu_config.xml";
	std::string xmltype = "name";
	hxmlFile = new rapidxml::file<>(xmlpath.c_str()); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

  	if (!mini){
		pNode = hpRoot->last_node("config")->first_node(xmltype.c_str());
	}
	else{
		pNode = hpRoot->first_node("config")->first_node(xmltype.c_str());
	}
	for (rapidxml::xml_node<> *bNode = pNode->first_node("textedit"); bNode; bNode = bNode->next_sibling("textedit"))
	{
		text_edits.push_back(TextEdit(
			rect.x + atoi(bNode->first_node("pos")->first_node("x")->value()),
			rect.y + atoi(bNode->first_node("pos")->first_node("y")->value()),
			atoi(bNode->first_node("pos")->first_node("w")->value()),
			atoi(bNode->first_node("pos")->first_node("h")->value()),
			255, 255, 255, SDL_BLENDMODE_BLEND, 0, color,
			pNode->first_node("font")->value(),
			atoi(pNode->first_node("fontsize")->value()),
			false));
	}

	for (rapidxml::xml_node<> *bNode = pNode->first_node("button"); bNode; bNode = bNode->next_sibling("button"))
	{
		controls.push_back(MeguButton(
			rect.x + atoi(bNode->first_node("pos")->first_node("x")->value()),
			rect.y + atoi(bNode->first_node("pos")->first_node("y")->value()),
			atoi(bNode->first_node("pos")->first_node("w")->value()),
			atoi(bNode->first_node("pos")->first_node("h")->value()), NULL, true));
	}
}

NameMenus::~NameMenus()
{
}

void NameMenus::disableMenu(){
	visible = false;
	controls[0].setEnable(true);
	controls[0].setVisible(true);
	controls[1].setEnable(false);
	controls[1].setVisible(false);
	for (std::vector<TextEdit>::iterator te_it = text_edits.begin(); te_it != text_edits.end(); ++te_it){
		te_it->setFocus(false);
		te_it->setVisible(false);
	}
}

void NameMenus::callMenu(){
	visible = true;
	controls[0].setEnable(true);
	controls[0].setVisible(true);;
	controls[1].setEnable(true);
	controls[1].setVisible(true);
	for (std::vector<TextEdit>::iterator te_it = text_edits.begin(); te_it != text_edits.end(); ++te_it){
		te_it->setVisible(true);
	}
}
void NameMenus::Render(SDL_Renderer *renderer)
{
	if (visible){
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		controls[0].Render(renderer);
		controls[1].Render(renderer);
		for (std::vector<TextEdit>::iterator te_it = text_edits.begin(); te_it != text_edits.end(); ++te_it){
			if (te_it->getText().empty()){
			}
			else{
				te_it->loadFromRenderedText(renderer);
				te_it->Render(renderer);
			}
		}
	}
}

SDL_Texture* NameMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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