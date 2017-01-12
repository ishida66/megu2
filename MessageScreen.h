#pragma once
#include "SDL_include.h"
#include "TtfWrapper.h"
#include <string>
#include <rapidXML/rapidxml.hpp>
#include <rapidXML/rapidxml_utils.hpp>
namespace widgets{
	class MessageScreen
	{
	public:
		MessageScreen();
		MessageScreen(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture, SDL_Renderer *renderer, const bool &mini);
		void addMessage(const std::string &_msg);
		TtfWrapper rendered_msg;
		SDL_Texture* loadTexture(std::string path, SDL_Renderer *renderer);
		void Render(SDL_Renderer *renderer);
		~MessageScreen();
		long getTimeout();
		void setTimeOut(const long &time);
		void setTimer(const long &time);
		long getTimer();
		bool visible;//if yes then render
		bool locked; //lock message on screen
	private:
		int font_size;
		SDL_Color color;
		long timer;
		long timeout;
		SDL_Rect msg_pos;
		SDL_Rect rect;
		std::string msg;
		SDL_Texture* texture;
		std::string font;
		//Load image at specified path
		
		rapidxml::file<> *hxmlFile;
		rapidxml::xml_document<> hdoc;
		rapidxml::xml_node<> *hpRoot;
		rapidxml::xml_node<> *gNode;
		rapidxml::xml_node<> *pNode;
		TtfWrapper *render_msg;
	};

}