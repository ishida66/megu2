#include "MessageScreen.h"
#include <iostream>
using namespace widgets;

MessageScreen::MessageScreen()
{
}


MessageScreen::~MessageScreen()
{
}
MessageScreen::MessageScreen(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini){
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	texture = newtexture;
	color.a = 50;
	color.b = 50;
	color.g = 50;
	color.r = 50;
	timer = 0;
	timeout = 0;
	hxmlFile = new rapidxml::file<>("assets//xml//message_plaque_config.xml");
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();
	if (!mini){
		gNode = hpRoot->last_node("config");
	}
	else{
		gNode = hpRoot->first_node("config");
	}
	pNode = gNode->first_node("message_plaque");
	msg_pos.x = atoi(pNode->first_node("message_text")->first_node("pos")->first_node("x")->value());
	msg_pos.y = atoi(pNode->first_node("message_text")->first_node("pos")->first_node("y")->value());
	font_size = atoi(pNode->first_node("message_text")->first_node("font_size")->value());
	font = pNode->first_node("message_text")->first_node("font")->value();
	render_msg = new TtfWrapper(rect.x + msg_pos.x, rect.y + msg_pos.y, 50, 50, 50, SDL_BLENDMODE_BLEND, 0, color, font_size, font,600, true);
	render_msg->loadFromRenderedText(renderer);
	setTimer(0);
	setTimeOut(0);
	visible = false;
	locked = false;
}

void MessageScreen::addMessage(const std::string &_msg){
	msg = _msg;
 	render_msg->text = msg;

}

SDL_Texture* MessageScreen::loadTexture(std::string path, SDL_Renderer *renderer){
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

long  MessageScreen::getTimeout(){
	return timeout;

}
void  MessageScreen::setTimeOut(const long &time){
	timeout = time;
}
void  MessageScreen::setTimer(const long &time){
	timer = time;
}
long MessageScreen::getTimer(){
	return timer;
}
void MessageScreen::Render(SDL_Renderer *renderer){
	if (visible){
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		render_msg->loadFromRenderedText(renderer);
		render_msg->Render(renderer);
	}
}