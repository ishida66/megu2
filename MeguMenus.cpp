#include "MeguMenus.h"
#include "SDL_include.h"
#include "Items_library.h"
using namespace widgets;

MeguMenus::MeguMenus(){
}
MeguMenus::MeguMenus(const int &x, const int &y, const  int &width, const  int &height, SDL_Texture *newtexture)
{
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	texture = newtexture;
	visible = false;
	SDL_Color color;
	color.r = 255;
	color.b = 255;
	color.a = 0;
	color.g = 255;

	current_page = 0;
}

void MeguMenus::disableMenu(){
	visible = false;
	for (widgets::button_container::iterator it = slots.begin(); it != slots.end(); ++it){
		it->setEnable(false);
		it->setVisible(false);
	}
	for (std::vector<TtfWrapper>::iterator it = slots_name.begin(); it != slots_name.end(); ++it){
		it->setVisible(false);
	}
	for (std::vector<TtfWrapper>::iterator it = slots_description.begin(); it != slots_description.end(); ++it){
		it->setVisible(false);
	}
	for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
		it->setVisible(false);
		it->setEnable(false);
	}
}

void MeguMenus::callMenu(){
	visible = true;
	for (widgets::button_container::iterator it = slots.begin(); it != slots.end(); ++it){
		it->setEnable(true);
		it->setVisible(true);
	}
	for (std::vector<TtfWrapper>::iterator it = slots_name.begin(); it != slots_name.end(); ++it){
		it->setVisible(true);
	}
	for (std::vector<TtfWrapper>::iterator it = slots_description.begin(); it != slots_description.end(); ++it){
		it->setVisible(true);
	}
	for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
		it->setVisible(true);
		it->setEnable(true);
	}
}
void MeguMenus::Render(SDL_Renderer *renderer)
{
	if (visible){
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		for (widgets::button_container::iterator it = slots.begin(); it != slots.end(); ++it){
			it->Render(renderer);
		}
		for (std::vector<TtfWrapper>::iterator it = slots_name.begin(); it != slots_name.end(); ++it){
			it->Render(renderer);
		}
		for (std::vector<TtfWrapper>::iterator it = slots_description.begin(); it != slots_description.end(); ++it){
			it->Render(renderer);
		}
		for (widgets::button_container::iterator it = controls.begin(); it != controls.end(); ++it){
			it->Render(renderer);
		}
	}
}

void MeguMenus::updateSlots(SDL_Renderer *renderer){}

void MeguMenus::setClassificationType(const CLASSIFICATION &classification, SDL_Renderer *renderer){
	current_classification = classification;
	updateSlots(renderer);
}
void MeguMenus::setViewPage(const int &page, SDL_Renderer *renderer){
	if (page <= (id_library[current_classification].size()-1)/3 && page >= 0){
		current_page = page;
		updateSlots(renderer);
	}

}

void MeguMenus::setCurrentId(const int &id){
	selected_id = id;
}

int MeguMenus::getCurrentId(){
	return selected_id;
}

MeguMenus::~MeguMenus()
{
}
