#include "OptionsMenus.h"
#include "SDL_include.h"
using namespace widgets;

OptionsMenus::OptionsMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini)
{

	msg_on = true;
	bgm_level = 5;
	sfx_level = 5;
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
	std::string xmlpath = "assets//xml//options_menu_config.xml";
	std::string xmltype = "options";
	rapidxml::file<>* hxmlFile;
	hxmlFile = new rapidxml::file<>(xmlpath.c_str()); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node(xmltype.c_str());
	}
	else{
		pNode = hpRoot->first_node("config")->first_node(xmltype.c_str());
	}
		//option buttons


	for (rapidxml::xml_node<> *dNode = pNode->first_node("slot"); dNode; dNode = dNode->next_sibling("slot")){
		option_buttons.push_back(MeguButton(rect.x + atoi(dNode->first_node("pos")->first_node("x")->value()), rect.y + atoi(dNode->first_node("pos")->first_node("y")->value()), atoi(dNode->first_node("pos")->first_node("w")->value()), atoi(dNode->first_node("pos")->first_node("h")->value()), NULL, true));
	}
	//control buttons aka back button
	for (rapidxml::xml_node<> *dNode = pNode->first_node("buttons"); dNode; dNode = dNode->next_sibling("buttons")){
		controls.push_back(MeguButton(rect.x + atoi(dNode->first_node("pos")->first_node("x")->value()), rect.y + atoi(dNode->first_node("pos")->first_node("y")->value()), atoi(dNode->first_node("pos")->first_node("w")->value()), atoi(dNode->first_node("pos")->first_node("h")->value()), NULL, true));
	}
	bgm_marker_pos.x = atoi(pNode->first_node("bgm_marker_pos")->first_node("pos")->first_node("x")->value());
	bgm_marker_pos.y = atoi(pNode->first_node("bgm_marker_pos")->first_node("pos")->first_node("y")->value());
	bgm_marker_offset = atoi(pNode->first_node("bgm_marker_pos")->first_node("offset")->first_node("x")->value());
	bgm_marker_cd.w = atoi(pNode->first_node("marker")->first_node("size")->first_node("w")->value());
	bgm_marker_cd.h = atoi(pNode->first_node("marker")->first_node("size")->first_node("h")->value());

	sfx_marker_pos.x = atoi(pNode->first_node("sfx_marker_pos")->first_node("pos")->first_node("x")->value());
	sfx_marker_pos.y = atoi(pNode->first_node("sfx_marker_pos")->first_node("pos")->first_node("y")->value());
	sfx_marker_offset = atoi(pNode->first_node("sfx_marker_pos")->first_node("offset")->first_node("x")->value());
	sfx_marker_cd.w = atoi(pNode->first_node("marker")->first_node("size")->first_node("w")->value());
	sfx_marker_cd.h = atoi(pNode->first_node("marker")->first_node("size")->first_node("h")->value());

	msg_on_pos.h = sfx_marker_cd.h;
	msg_on_pos.w = sfx_marker_cd.w;
	marker = loadTexture(pNode->first_node("marker")->first_node("texture")->value(), renderer);

}

void OptionsMenus::RenderButtons(SDL_Renderer *renderer)
{
}

OptionsMenus::~OptionsMenus()
{
}
void OptionsMenus::setBGMLevel(const int&bgm){
	if (bgm < 6 && bgm > -1){
		bgm_level = bgm;
	}
}
void OptionsMenus::setSFXLevel(const int&sfx){
	if (sfx < 6 && sfx > -1){
		sfx_level = sfx;
	}
}
void OptionsMenus::setMSGOn(const bool&msg){
	msg_on = msg;
}
int OptionsMenus::getBGMLevel()
{
	return bgm_level;
}
int OptionsMenus::getSFXLevel(){
	return sfx_level;
}
bool OptionsMenus::getMSGOn(){
	return msg_on;

}
void OptionsMenus::updateMarkers(){
	bgm_marker_cd.x = bgm_marker_pos.x + bgm_level*bgm_marker_offset;
	bgm_marker_cd.y = bgm_marker_pos.y;
	sfx_marker_cd.x = sfx_marker_pos.x + sfx_level*sfx_marker_offset;
	sfx_marker_cd.y = sfx_marker_pos.y;

}
SDL_Texture* OptionsMenus::loadTexture(std::string path, SDL_Renderer *renderer)
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


void OptionsMenus::disableMenu(){
	visible = false;

	for (widgets::button_container::iterator it = option_buttons.begin(); it != option_buttons.end(); ++it){
		it->setVisible(false);
	}
	for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
		it->setVisible(false);
		it->setEnable(false);
	}
}

void OptionsMenus::callMenu(){
	visible = true;
	updateMarkers();
	for (widgets::button_container::iterator it = option_buttons.begin(); it != option_buttons.end(); ++it){
		it->setVisible(true);
	}

	for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
		it->setVisible(true);
		it->setEnable(true);
	}

}
void OptionsMenus::Render(SDL_Renderer *renderer)
{
	if (visible){
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		for (widgets::button_container::iterator it = option_buttons.begin(); it != option_buttons.end(); ++it){
			it->Render(renderer);
		}
		for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
			it->Render(renderer);
		}

		if (msg_on){
			msg_on_pos.x = option_buttons[4].getX();
			msg_on_pos.y = option_buttons[4].getY();
			SDL_RenderCopy(renderer, marker, 0, &msg_on_pos);
		}
		else{
			msg_on_pos.x = option_buttons[5].getX();
			msg_on_pos.y = option_buttons[5].getY();
			SDL_RenderCopy(renderer, marker, 0, &msg_on_pos);
		}
		SDL_RenderCopy(renderer, marker, 0, &bgm_marker_cd);
		SDL_RenderCopy(renderer, marker, 0, &sfx_marker_cd);
	}
}