#include "MeguEngine.h"
#include "MeguSound.h"
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#include <sys/types.h>
#include <sys/dir.h>
#endif
#ifdef _WIN32
#include "dirent.h"
#endif
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include "SDL_include.h"
#include<rapidXML/rapidxml.hpp>
#include<rapidXML/rapidxml_utils.hpp>

MeguEngine::MeguEngine() : megu_blink_counter(1500),
megu_animation_counter(1000),
megu_misc_counter(5000),
megu_event_action(10000),
scene_limit(320),
megu_openeye_counter(50),
megu_pet_counter(150),
megu_spd(20),
megu_msg_counter(30000),
megu_heart_event_counter(250)
{
	Running = true;
	curr_time = 0;
	last_time = 0;
	mPosition.x = 0;
	mPosition.y = 0;
	menu_open = false;
	player_counter = 0;
	msg_time = 0;
	id_accessory = 0;
}


bool sortfunct(render_ordering i, render_ordering j){
	return (i.position_y < j.position_y);
}

bool sortobjects(Megu::scene_objects i, Megu::scene_objects j){
	return (i.render_id < j.render_id);
}

void MeguEngine::SetRenderOrder(){
	render_order.clear();
	for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator sprite_it = megu_list.begin(); sprite_it != megu_list.end(); ++sprite_it){
		render_order.push_back(render_ordering(sprite_it->first, sprite_it->second.position.y));
	}
	std::sort(render_order.begin(), render_order.end(), sortfunct);
}

void MeguEngine::initMenus(){
	rapidxml::file<> *hxmlFile; 
	hxmlFile = new rapidxml::file<>("assets//xml//main_screen_config.xml"); // Default template is char
	rapidxml::xml_document<> hdoc;
	hdoc.parse<0>(hxmlFile->data());
	rapidxml::xml_node<> *hpRoot = hdoc.first_node();
	rapidxml::xml_node<> *pNode;
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("main_menu");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("main_menu");
	}
	//load menus

	menu_bar_pos.w = atoi(pNode->first_node("menu_bar")->first_node("pos")->first_node("w")->value());
	menu_bar_pos.h = atoi(pNode->first_node("menu_bar")->first_node("pos")->first_node("h")->value());
	menu_bar_pos.x = atoi(pNode->first_node("menu_bar")->first_node("pos")->first_node("x")->value());
	menu_bar_pos.y = atoi(pNode->first_node("menu_bar")->first_node("pos")->first_node("y")->value());
	menu_background_pos.x = atoi(pNode->first_node("background")->first_node("pos")->first_node("x")->value());
	menu_background_pos.y = atoi(pNode->first_node("background")->first_node("pos")->first_node("y")->value());
	menu_background_pos.w = atoi(pNode->first_node("background")->first_node("pos")->first_node("w")->value());
	menu_background_pos.h = atoi(pNode->first_node("background")->first_node("pos")->first_node("h")->value());	

	
	edit_holding = false;
	//loading xml files
	
	
	
    std::cerr << "mouse cursor loading" << std::endl;
	mouse_cursor = loadTexture("assets/cursor/hand.png");
	menu_bar = loadTexture(pNode->first_node("menu_bar")->first_node("texture")->value());
	updateBackground(1);
    std::cerr << "populate libs " << std::endl;
	library.populate(renderer);
    std::cerr <<"load buttons" << std::endl;
	care_button = new widgets::MeguButton(
		  atoi(pNode->first_node("care_button")->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("care_button")->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("care_button")->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("care_button")->first_node("pos")->first_node("h")->value())
		, NULL
		, true
		);
	edit_button = new widgets::MeguButton(
		atoi(pNode->first_node("edit_button")->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("edit_button")->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("edit_button")->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("edit_button")->first_node("pos")->first_node("h")->value())
		, NULL
		, true
		);
	option_button = new widgets::MeguButton(
		atoi(pNode->first_node("option_button")->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("option_button")->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("option_button")->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("option_button")->first_node("pos")->first_node("h")->value())
		, NULL
		, true
		);
	return_button = new widgets::MeguButton(
		atoi(pNode->first_node("return_button")->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("return_button")->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("return_button")->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("return_button")->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("return_button")->first_node("texture")->value())
		, true);
	return_button->setEnable(false);
	return_button->setVisible(false);
    std::cerr << "load house objects" << std::endl;
    std::cerr << "populate house libs" << std::endl;
	
	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//house_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();
	
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("house");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("house");
	}
	house_menu = new widgets::HouseMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.house_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
	populateHouseLib(*house_menu, library.house_lib);
    std::cerr << "load food objects" << std::endl;
    std::cerr << "populate food libs" << std::endl;
	
	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//food_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("food");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("food");
	}
	food_menu = new widgets::FoodMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.food_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
	populateLib(*food_menu, library.food_lib);
    std::cerr << "load furniture objects" << std::endl;
    std::cerr << "populate furniture libs" << std::endl;

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//furniture_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("furniture");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("furniture");
	}
	furniture_menu = new widgets::FurnitureMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.furniture_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);

	populateLib(*furniture_menu, library.furniture_lib);

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//accessory_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("accessory");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("accessory");
	}
	accessory_menu = new widgets::AccessoryMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.accessory_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);

	populateLib(*accessory_menu, library.accessory_lib);
    std::cerr << "load pet objects" << std::endl;
    std::cerr << "populate pet libs" << std::endl;

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//megutate_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("megutate");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("megutate");
	}
	pet_menu = new widgets::PetMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.megu_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);

	populatePetLib(*pet_menu, library.megu_lib);

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//friends_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("friends");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("friends");
	}

	friends_menu = new widgets::FriendsMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, &library.megu_lib
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
	populatePetLib(*friends_menu, library.megu_lib);

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//layout_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("layout");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("layout");
	}

	layout_menu = new widgets::LayoutMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
	loadLayouts();
	food_menu->setClassificationType("American", renderer);
	furniture_menu->setClassificationType("furniture", renderer);
	pet_menu->setClassificationType("megu", renderer);
	house_menu->setClassificationType("house", renderer);
	accessory_menu->setClassificationType("accessory", renderer);
	food_menu->setViewPage(0, renderer);
	furniture_menu->setViewPage(0, renderer);
	pet_menu->setViewPage(0, renderer);
	house_menu->setViewPage(0, renderer);
	accessory_menu->setViewPage(0, renderer);
    std::cerr << "start menus" << std::endl;

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//care_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("care");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("care");
	}
	care_menu = new widgets::CareMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//edit_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("edit");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("edit");
	}
	edit_menu = new widgets::EditMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//name_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();

	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("name");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("name");
	}
	name_menu = new widgets::NameMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);


	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//options_menu_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();
	//*Change this portion
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("options");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("options");
	}
	options_menu = new widgets::OptionsMenus(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
    std::cerr << "load friend list" << std::endl;
    loadFriendList();
    std::cerr << "parse friend list" << std::endl;
	friends_menu->parseFiles(friends_save_list);
	layout_menu->parseFiles(layout_save_list);
	//food kit init
	kit.table = (*library.furniture_lib[1].texture.begin());

	delete hxmlFile;
	hxmlFile = new rapidxml::file<>("assets//xml//main_screen_config.xml"); // Default template is char
	hdoc.parse<0>(hxmlFile->data());
	hpRoot = hdoc.first_node();
	//*Change this portion
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("food_kit");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("food_kit");
	}
	kit.table_shadow = loadTexture(pNode->first_node("shadow")->first_node("texture")->value());
	SDL_SetTextureAlphaMod(kit.table_shadow, 46);
	int w, h;
	kit.rect_t.h = atoi(pNode->first_node("pos")->first_node("h")->value());
	kit.rect_t.w = atoi(pNode->first_node("pos")->first_node("w")->value());
	kit.rect_t.x = atoi(pNode->first_node("pos")->first_node("x")->value()) - kit.rect_t.w / 2;
	kit.rect_t.y = atoi(pNode->first_node("pos")->first_node("y")->value());

	kit.rect_s.x = kit.rect_t.x;
	kit.rect_s.y = kit.rect_t.y + kit.rect_t.h + atoi(pNode->first_node("shadow")->first_node("pos")->first_node("y")->value());
	kit.rect_s.h = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("h")->value());
	kit.rect_s.w = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("w")->value());
	
	kit.even_megu.x = atoi(pNode->first_node("megu_even_pos")->first_node("x")->value());
	kit.even_megu.y = atoi(pNode->first_node("megu_even_pos")->first_node("y")->value());
	kit.odd_megu.x = atoi(pNode->first_node("megu_odd_pos")->first_node("x")->value());
	kit.odd_megu.y = atoi(pNode->first_node("megu_odd_pos")->first_node("y")->value());
	kit.rect_f.x = kit.rect_t.x + atoi(pNode->first_node("food")->first_node("pos")->first_node("x")->value());
	kit.rect_f.y = kit.rect_t.y + atoi(pNode->first_node("food")->first_node("pos")->first_node("y")->value());
	kit.rect_f.w = atoi(pNode->first_node("food")->first_node("pos")->first_node("w")->value());
	kit.rect_f.h = atoi(pNode->first_node("food")->first_node("pos")->first_node("h")->value());
	
	kit.visible = false;
	SDL_Color color;
	color.a = 255;
	color.b = 230;
	color.g = 230;
	color.r = 230;
	if (!mini){
		pNode = hpRoot->last_node("config")->first_node("main_menu");
	}
	else{
		pNode = hpRoot->first_node("config")->first_node("main_menu");
	}
	ui_texts.push_back(TtfWrapper(menu_bar_pos.x + atoi(pNode->first_node("name_input")->first_node("pos")->first_node("x")->value())
		, menu_bar_pos.y + atoi(pNode->first_node("name_input")->first_node("pos")->first_node("y")->value())
		, 255
		, 255
		, 255
		, SDL_BLENDMODE_BLEND
		, 0
		, color
		, atoi(pNode->first_node("name_input")->first_node("font_size")->value())
		, pNode->first_node("owner_input")->first_node("font")->value()
		, 300
		, false)
		);
	ui_texts.push_back(TtfWrapper(menu_bar_pos.x + atoi(pNode->first_node("owner_input")->first_node("pos")->first_node("x")->value())
		, menu_bar_pos.y + atoi(pNode->first_node("owner_input")->first_node("pos")->first_node("y")->value())
		, 255
		, 255
		, 255
		, SDL_BLENDMODE_BLEND
		, 0
		, color
		, atoi(pNode->first_node("owner_input")->first_node("font_size")->value())
		, pNode->first_node("owner_input")->first_node("font")->value()
		, 300
		, false)
		); 
}

bool MeguEngine::OnInit() {
	mini = true;
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    
    chdir(path);
    std::cout << "Current Path: " << path << std::endl;
#endif
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "SDL initializing" << std::endl;
		return false;
	}
	if (TTF_Init() == -1){
		std::cerr << "SDL_TTF error: " << TTF_GetError() << std::endl;
		exit(2);
	}
	std::cerr << "open audio" << std::endl;
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		return false;
	}
	sounds.loadAll();
	//init window
	std::cerr << "create window" << std::endl;
	win = SDL_CreateWindow("MEGU 2.2!", 100, 100, 180, 100, 0);
	//init render
	std::cerr << "set renderer" << std::endl;
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	rapidxml::file<> shxmlFile("assets//xml//size_menu_config.xml"); // Default template is char
	rapidxml::xml_document<> shdoc;
	shdoc.parse<0>(shxmlFile.data());
	rapidxml::xml_node<> *shpRoot = shdoc.first_node();
	rapidxml::xml_node<> *spNode;
	status = Game::INIT;
	spNode = shpRoot->first_node("config")->first_node("size_selector");
	sounds.setBGMVolume(3);
	sounds.playBGM("stroll");
	size_menu = new widgets::SizeMenus(
		atoi(spNode->first_node("pos")->first_node("x")->value())
		, atoi(spNode->first_node("pos")->first_node("y")->value())
		, atoi(spNode->first_node("pos")->first_node("w")->value())
		, atoi(spNode->first_node("pos")->first_node("h")->value())
		, loadTexture(spNode->first_node("texture")->value())
		, renderer);
	size_menu->callMenu();
	SDL_Event Event;
	while (status == Game::INIT) {
		
		while (SDL_PollEvent(&Event)) {
			handleSizeSelector(&Event);
		}
		size_menu->Render(renderer);
		SDL_RenderPresent(renderer);
	}
	size_menu->disableMenu();

	delete size_menu;

	int win_h, win_w;
	rapidxml::file<> hxmlFile("assets//xml//game_window_config.xml"); // Default template is char
	rapidxml::xml_document<> hdoc;
	hdoc.parse<0>(hxmlFile.data());
	rapidxml::xml_node<> *hpRoot = hdoc.first_node();
	rapidxml::xml_node<> *pNode;
	if (!mini){
		pNode = hpRoot->last_node("config");
	}
	else{
		pNode = hpRoot->first_node("config");
	}
	win_w = atoi(pNode->first_node("game_window")->first_node("size")->first_node("w")->value());
	win_h = atoi(pNode->first_node("game_window")->first_node("size")->first_node("h")->value());

	SDL_SetWindowSize(win, win_w, win_h);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
    std::cerr << "init menus" << std::endl;
    initMenus();
	//init other objects
	initObjects();
	populateRndMsg();
	deserializeOptions();
    std::cerr << "game running" << std::endl;
	status = Game::RUNNING;
	return true;

}
void MeguEngine::initObjects(){
	rapidxml::file<> *hxmlFile = new rapidxml::file<>("assets//xml//main_screen_config.xml"); // Default template is char
	rapidxml::xml_document<> hdoc;
	hdoc.parse<0>(hxmlFile->data());
	rapidxml::xml_node<> *hpRoot = hdoc.first_node();
	rapidxml::xml_node<> *pNode;
	rapidxml::xml_node<> *gNode;
	rapidxml::xml_node<> *cNode;
	if (!mini){
		gNode = hpRoot->last_node("config");
	}
	else{
		gNode = hpRoot->first_node("config");
	}
	pNode = gNode->first_node("game_models")->first_node("megu");
	megu_size.x = atoi(pNode->first_node("init_pos")->first_node("x")->value());
	megu_size.y = atoi(pNode->first_node("init_pos")->first_node("y")->value());
	megu_size.w = atoi(pNode->first_node("init_pos")->first_node("w")->value());
	megu_size.h = atoi(pNode->first_node("init_pos")->first_node("h")->value());
	shadow_pos.x = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("x")->value());
	shadow_pos.y = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("y")->value());
	shadow_pos.w = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("w")->value());
	shadow_pos.h = atoi(pNode->first_node("shadow")->first_node("pos")->first_node("h")->value());
	shadow_texture = pNode->first_node("shadow")->first_node("texture")->value();
	shadow_alpha = atoi(pNode->first_node("shadow")->first_node("texture")->value());

	clickable_area.x = atoi(gNode->first_node("walk_area")->first_node("pos")->first_node("x")->value());
	clickable_area.y = atoi(gNode->first_node("walk_area")->first_node("pos")->first_node("y")->value());
	clickable_area.w = atoi(gNode->first_node("walk_area")->first_node("pos")->first_node("w")->value());
	clickable_area.h = atoi(gNode->first_node("walk_area")->first_node("pos")->first_node("h")->value());
	
	delete hxmlFile;
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
	message_screen = new widgets::MessageScreen(
		atoi(pNode->first_node("pos")->first_node("x")->value())
		, atoi(pNode->first_node("pos")->first_node("y")->value())
		, atoi(pNode->first_node("pos")->first_node("w")->value())
		, atoi(pNode->first_node("pos")->first_node("h")->value())
		, loadTexture(pNode->first_node("texture")->value())
		, renderer
		, mini);
	if (!mini){
		hearts_texture.push_back(loadTexture("assets/objects/heart_s@2x.png"));
		hearts_texture.push_back(loadTexture("assets/objects/heart_m@2x.png"));
		hearts_texture.push_back(loadTexture("assets/objects/heart_l@2x.png"));
		heart_offset.x = 128;
		heart_offset.y = -50;
	}
	else{
		hearts_texture.push_back(loadTexture("assets/objects/heart_s.png"));
		hearts_texture.push_back(loadTexture("assets/objects/heart_m.png"));
		hearts_texture.push_back(loadTexture("assets/objects/heart_l.png"));
		heart_offset.x = 64;
		heart_offset.y = -25;
	}

}


void MeguEngine::setMousePosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void MeguEngine::handleSizeSelector(SDL_Event* e){
	int x, y;
	SDL_GetMouseState(&x, &y);
	switch (e->type){
	case SDL_MOUSEBUTTONDOWN:
		if (size_menu->buttons.at(0).IsIn(x, y)){
			mini = false;
			status = Game::RUNNING;
		}
		else if (size_menu->buttons.at(1).IsIn(x, y)){
			mini = true;
			status = Game::RUNNING;
		}
		break;
	}
}
void MeguEngine::handleMouseEvent(SDL_Event* e)
{
	//Get mouse position
	int x, y;
	SDL_GetMouseState(&x, &y);
	//mouse event
	//verify typing
	if (name_menu->text_edits.at(0).getFocus()){
		name_menu->text_edits.at(0).handleKey(e);
	}
	else if (name_menu->text_edits.at(1).getFocus()){
		name_menu->text_edits.at(1).handleKey(e);
	}
	switch (e->type){

	case SDL_MOUSEBUTTONDOWN:

		if (status == Game::RUNNING){
			//Check if mouse is in button
			for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator sprite_it = megu_list.begin(); sprite_it != megu_list.end(); ++sprite_it){
				std::cerr << "mouse coords: " << x << " " << y << std::endl;
				if (care_button->IsIn(x, y)){
					sounds.playSFX(std::string("click"), 5);
					care_menu->callMenu();
					status = Game::CARE_MENU;
				}
				else if (edit_button->IsIn(x, y)){
					sounds.playSFX("click", 5);
					edit_menu->callMenu();
					status = Game::EDIT_MENU;
				}
				else if (option_button->IsIn(x, y)){
					sounds.playSFX("click", 5);
					options_menu->callMenu();
					status = Game::OPTIONS_MENU;
				}
				if (e->button.button == SDL_BUTTON_LEFT){
					if ((x > sprite_it->second.position.x) && (x < sprite_it->second.position.x + sprite_it->second.position.w) &&
						(y > sprite_it->second.position.y) && (y < sprite_it->second.position.y + sprite_it->second.position.h)){
						sprite_it->second.pet_flag = true;
					}
					else if (y > clickable_area.y && y <  clickable_area.y + clickable_area.h && x > clickable_area.x && x < clickable_area.x + clickable_area.w){
						for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator it = megu_list.begin(); it != megu_list.end(); ++it){
							if ((it->first) % 2 == 1){
								MeguClickScene(it->first, x - megu_size.w / 2, y - megu_size.h);
								if (e->button.clicks == 1){
									it->second.run_flag = false;
								}
								else if (e->button.clicks == 2){
									it->second.run_flag = true;
								}
							}
						}

					}

				}
				if (e->button.button == SDL_BUTTON_RIGHT){
					if ((x > sprite_it->second.position.x) && (x < sprite_it->second.position.x + sprite_it->second.position.w) &&
						(y > sprite_it->second.position.y) && (y < sprite_it->second.position.y + sprite_it->second.position.h)){
						sprite_it->second.pet_flag = true;
					}
					else if (y > clickable_area.y && y <  clickable_area.y + clickable_area.h && x > clickable_area.x && x < clickable_area.x + clickable_area.w){
						for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator it = megu_list.begin(); it != megu_list.end(); ++it){
							if ((it->first) % 2 == 0){
								MeguClickScene(it->first, x - megu_size.w / 2, y - megu_size.h);
								if (e->button.clicks == 1){
									it->second.run_flag = false;
								}
								else if (e->button.clicks == 2){
									it->second.run_flag = true;
								}
							}
						}
					}
				}

			}
		}

		else if (status == Game::CARE_MENU){
			if (care_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				care_menu->disableMenu();
			}
			if (care_menu->slots[0].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				food_menu->callMenu();
				food_menu->updateSlots(renderer);
				status = Game::FOOD_MENU;
			}
			if (care_menu->slots[1].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				furniture_menu->callMenu();
				furniture_menu->updateSlots(renderer);
				status = Game::FURNITURE_MENU;
			}
			else if (care_menu->slots[2].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				status = Game::EDIT_MODE;
				return_button->setEnable(true);
				return_button->setVisible(true);
				message_screen->addMessage("Click to pick. DEL to delete. PGUP PGDN to organize");
				message_screen->visible = true;
				message_screen->locked = true;
			}
			else if (care_menu->slots[3].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				pet_menu->callMenu();
				pet_menu->updateSlots(renderer);
				status = Game::PET_MENU;
				sounds.pauseMusic();
				sounds.playSFX("changing", 5);
			}
			else if (care_menu->slots[4].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				objects_list.clear();
				status = Game::HOUSE_MENU;
				status = Game::RUNNING;
			}
			else if (care_menu->slots[5].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				house_menu->callMenu();
				house_menu->updateSlots(renderer);
				sounds.pauseMusic();
				sounds.playSFX("Continuation", 5);
				status = Game::HOUSE_MENU;
			}
/*			else if (care_menu->slots[6].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				accessory_menu->callMenu();
				accessory_menu->updateSlots(renderer);
				status = Game::ACCESSORY_MENU;
			}
			else if (care_menu->slots[7].IsIn(x, y)){
				care_menu->disableMenu();
				sounds.playSFX("click", 5);
				return_button->setEnable(true);
				return_button->setVisible(true);
				message_screen->addMessage("Click to pick. DEL to delete. PGUP PGDN to organize");
				message_screen->visible = true;
				message_screen->locked = true;
				SetMeguPos(1, 192, 192, SDL_FLIP_NONE);
				OnRender();
				status = Game::ACCESSORY_MODE;
			}
			*/
		}

		else if (status == Game::EDIT_MENU){
			if (edit_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				edit_menu->disableMenu();
			}
			if (edit_menu->slots[0].IsIn(x, y)){
				edit_menu->disableMenu();
				name_menu->text_edits.at(0).setText(megu_list[1].owner_name);
				name_menu->text_edits.at(1).setText(megu_list[1].pet_name);
				name_menu->callMenu();
				status = Game::NAME_MENU;
			}

			if (edit_menu->slots[1].IsIn(x, y)){

				edit_menu->disableMenu();
				sounds.playSFX("click", 5);
				loadLayouts();
				layout_menu->parseFiles(layout_save_list);
				layout_menu->callMenu();
				layout_menu->setMode(true);
				layout_menu->updateSlots(renderer);

				status = Game::LAYOUT_SAVE_MENU;
			}

			else if (edit_menu->slots[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				loadLayouts();
				if (!layout_save_list.empty()){
					edit_menu->disableMenu();
					layout_menu->parseFiles(layout_save_list);
					layout_menu->callMenu();
					layout_menu->updateSlots(renderer);
					layout_menu->setMode(false);
					status = Game::LAYOUT_LOAD_MENU;
				}
			}

			else if (edit_menu->slots[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				edit_menu->disableMenu();
				sounds.playSFX("click", 5);
				pet_menu->callMenu();
				status = Game::NEW_MEGU_PET_MENU;


			}
			else if (edit_menu->slots[4].IsIn(x, y)){
				edit_menu->disableMenu();
				friends_menu->parseFiles(friends_save_list);
				friends_menu->callMenu();
				friends_menu->updateSlots(renderer);
				sounds.playSFX("click", 5);
				status = Game::FRIEND_INVITE_MENU;
			}
			else if (edit_menu->slots[5].IsIn(x, y)){
				sounds.playSFX("Departure", 5);
				if (megu_list.size() > 1){
					std::vector<int> id_list;
					for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator it = megu_list.begin(); it != megu_list.end(); ++it){
						if (it->first != 1){//except user
							id_list.push_back(it->first);
						}
					}
					render_order.clear();
					render_order.push_back(render_ordering(1, megu_list[1].position.y));

					for (std::vector<int>::iterator r_it = id_list.begin(); r_it != id_list.end(); ++r_it){
						std::map<PLAYER_ID, Megu::megu_sprites>::iterator ID_iterator;
						ID_iterator = megu_list.find(*r_it);
						megu_list.erase(ID_iterator, megu_list.end());
					}
				}
				edit_menu->disableMenu();
				status = Game::RUNNING;


			}
			else if (edit_menu->slots[6].IsIn(x, y)){
				edit_menu->disableMenu();
				sounds.playSFX("click", 5);
				serializePlayer("gamedata/friends/player.megu", megu_list[1]);
				status = Game::RUNNING;
			}

		}
		else if (status == Game::OPTIONS_MENU){
			if (options_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				serializeOptions();
				options_menu->disableMenu();
				status = Game::RUNNING;
			}
			else if (options_menu->option_buttons[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setSFXLevel(options_menu->getSFXLevel() - 1);
				sounds.setSFXVolume(options_menu->getSFXLevel());
				options_menu->updateMarkers();
			}
			else if (options_menu->option_buttons[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setSFXLevel(options_menu->getSFXLevel() + 1);
				sounds.setSFXVolume(options_menu->getSFXLevel());
				options_menu->updateMarkers();
			}
			else if (options_menu->option_buttons[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setBGMLevel(options_menu->getBGMLevel() - 1);
				sounds.setBGMVolume(options_menu->getBGMLevel());
				options_menu->updateMarkers();
			}
			else if (options_menu->option_buttons[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setBGMLevel(options_menu->getBGMLevel() + 1);
				sounds.setBGMVolume(options_menu->getBGMLevel());
				options_menu->updateMarkers();
			}
			else if (options_menu->option_buttons[4].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setMSGOn(true);
				options_menu->updateMarkers();
			}
			else if (options_menu->option_buttons[5].IsIn(x, y)){
				sounds.playSFX("click", 5);
				options_menu->setMSGOn(false);
				options_menu->updateMarkers();
			}

		}
		else if (status == Game::NAME_MENU){
			if (name_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				name_menu->disableMenu();
				status = Game::RUNNING;
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(false);
			}
			else if (name_menu->text_edits.at(0).isIn(x, y)){
				name_menu->text_edits.at(0).setFocus(true);
				name_menu->text_edits.at(1).setFocus(false);
			}
			else if (name_menu->text_edits.at(1).isIn(x, y)){
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(true);
			}
			else if (name_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				megu_list[1].owner_name = name_menu->text_edits.at(0).getText();
				megu_list[1].pet_name = name_menu->text_edits.at(1).getText();
				status = Game::RUNNING;
				ui_texts[0].text = megu_list[1].owner_name;
				ui_texts[1].text = megu_list[1].pet_name;
				for (std::vector<TtfWrapper>::iterator it = ui_texts.begin(); it != ui_texts.end(); ++it){
					it->loadFromRenderedText(renderer);
				}
				name_menu->disableMenu();
			}
			else{
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(false);
			}
		}
		else if (status == Game::HOUSE_MENU){
			if (house_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				sounds.cutSFX(6);
				house_menu->disableMenu();
				if (sounds.getBGMOn())
					sounds.resumeMusic();
				status = Game::RUNNING;
			}
			else if (house_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				house_menu->marker_visible = false;
				updateBackground(house_menu->getCurrentId());
				sounds.cutSFX(6);
				house_menu->disableMenu();
				sounds.playSFX("GetCntinuation", 5);
				if (sounds.getBGMOn())
					sounds.resumeMusic();
				OnRender();
				serializeHouse("DEFAULT");
				status = Game::RUNNING;
			}
			else if (house_menu->controls[1].IsIn(x, y)){
				house_menu->setViewPage(house_menu->current_page - 1, renderer);
				house_menu->marker_visible = false;
				house_menu->controls[3].setEnable(false);
				sounds.playSFX("click", 5);

			}
			else if (house_menu->controls[0].IsIn(x, y)){
				house_menu->setViewPage(house_menu->current_page + 1, renderer);
				house_menu->marker_visible = false;
				house_menu->controls[3].setEnable(false);
				sounds.playSFX("click", 5);
			}
			//item selector
			for (widgets::button_container::iterator it = house_menu->slots.begin(); it != house_menu->slots.end(); ++it){
				if (it->IsIn(x, y)){
					if (it->getEnable()){
						house_menu->setCurrentId(it->getId());
						house_menu->marker_rect.x = it->getX();
						house_menu->marker_rect.y = it->getY();
						house_menu->marker_visible = true;
						house_menu->controls[3].setEnable(true);

					}
				}
			}
		}
		else if (status == Game::PET_MENU){
			if (pet_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				sounds.cutSFX(3);
				if (sounds.getBGMOn())
					sounds.resumeMusic();
				pet_menu->marker_visible = false;
				pet_menu->disableMenu();
			}
			if (pet_menu->controls[3].IsIn(x, y)){
				int player = 0;
				if (e->button.button == SDL_BUTTON_LEFT){
					player = 1;
				}

				sounds.playSFX("click", 5);
				int x = megu_list[player].position.x;
				int y = megu_list[player].position.y;
				SDL_RendererFlip flip = megu_list[player].side;
				LoadMegu(pet_menu->getCurrentId(), player);
				SetMeguPos(player, x, y, flip);
				SetMeguState(player, Megu::VERY_HAPPY);
				sounds.cutSFX(3);
				if (sounds.getBGMOn())
					sounds.resumeMusic();
				sounds.playSFX("changed", 5);
				pet_menu->marker_visible = false;
				ui_texts[0].text = megu_list[1].owner_name;
				ui_texts[1].text = megu_list[1].pet_name;
				//ui_texts[2].text = megu_list[1].pet_race;
				for (std::vector<TtfWrapper>::iterator it = ui_texts.begin(); it != ui_texts.end(); ++it){
					it->loadFromRenderedText(renderer);
				}
				pet_menu->disableMenu();
				status = Game::RUNNING;
			}
			else if (pet_menu->controls[1].IsIn(x, y)){
				pet_menu->setViewPage(pet_menu->current_page - 1, renderer);
				pet_menu->marker_visible = false;
				pet_menu->controls[3].setEnable(false);
			}
			else if (pet_menu->controls[0].IsIn(x, y)){
				pet_menu->setViewPage(pet_menu->current_page + 1, renderer);
				pet_menu->marker_visible = false;
				pet_menu->controls[3].setEnable(false);
			}
			//item selector

			for (widgets::button_container::iterator it = pet_menu->slots.begin(); it != pet_menu->slots.end(); ++it){
				if (it->IsIn(x, y)){
					if (it->getEnable()){
						pet_menu->setCurrentId(it->getId());
						pet_menu->marker_rect.x = it->getX();
						pet_menu->marker_rect.y = it->getY();
						pet_menu->marker_visible = true;
						pet_menu->controls[3].setEnable(true);
					}
				}
			}
		}

		else if (status == Game::NEW_MEGU_NAME_MENU){
			if (name_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				name_menu->disableMenu();
				new_megu_id = 0;
				status = Game::RUNNING;
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(false);
			}
			else if (name_menu->text_edits.at(0).isIn(x, y)){
				name_menu->text_edits.at(0).setFocus(true);
				name_menu->text_edits.at(1).setFocus(false);
			}
			else if (name_menu->text_edits.at(1).isIn(x, y)){
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(true);
			}
			else if (name_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				int p_id = megu_list.rbegin()->first + 1;
				LoadMegu(new_megu_id, p_id);
				megu_list[p_id].owner_name = name_menu->text_edits.at(0).getText();
				megu_list[p_id].pet_name = name_menu->text_edits.at(1).getText();
				megu_list[p_id].position.x = megu_size.x;
				megu_list[p_id].position.y = megu_size.y;
				megu_list[p_id].shadow_pos.x = megu_size.x + shadow_pos.x;
				megu_list[p_id].shadow_pos.y = megu_size.y + shadow_pos.y;

				megu_list[p_id].blink = Megu::OPEN;
				megu_list[p_id].state = Megu::STOP_ONE;
				serializePlayer("gamedata/friends/" + megu_list[p_id].pet_race + "_" + megu_list[p_id].owner_name + "_" + megu_list[p_id].pet_name + ".megu", megu_list[p_id]);
				name_menu->disableMenu();
				loadFriendList();
				sounds.playSFX("Complete", 5);
				status = Game::RUNNING;
				OnRender();
			}
			else{
				name_menu->text_edits.at(0).setFocus(false);
				name_menu->text_edits.at(1).setFocus(false);
			}
		}
		else if (status == Game::NEW_MEGU_PET_MENU){
			if (pet_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				sounds.cutSFX(3);
				if (sounds.getBGMOn())
					sounds.resumeMusic();
				pet_menu->marker_visible = false;
				pet_menu->disableMenu();
			}
			if (pet_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				pet_menu->disableMenu();
				name_menu->text_edits[0].setText(megu_list[1].owner_name);
				name_menu->text_edits[1].setText("MEGU");
				name_menu->callMenu();
				status = Game::NEW_MEGU_NAME_MENU;
			}
			else if (pet_menu->controls[1].IsIn(x, y)){
				pet_menu->setViewPage(pet_menu->current_page - 1, renderer);
				pet_menu->marker_visible = false;
				pet_menu->controls[3].setEnable(false);
			}
			else if (pet_menu->controls[0].IsIn(x, y)){
				pet_menu->setViewPage(pet_menu->current_page + 1, renderer);
				pet_menu->marker_visible = false;
				pet_menu->controls[3].setEnable(false);
			}
			//item selector

			for (widgets::button_container::iterator it = pet_menu->slots.begin(); it != pet_menu->slots.end(); ++it){
				if (it->IsIn(x, y)){
					if (it->getEnable()){
						new_megu_id = it->getId();
						pet_menu->marker_rect.x = it->getX();
						pet_menu->marker_rect.y = it->getY();
						pet_menu->marker_visible = true;
						pet_menu->controls[3].setEnable(true);
					}
				}
			}
		}


		else if (status == Game::FURNITURE_MENU){
			if (furniture_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				furniture_menu->disableMenu();
			}
			if (furniture_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::EDIT_MODE;
				return_button->setEnable(true);
				return_button->setVisible(true);
				message_screen->addMessage("Click to pick. DEL to delete. PGUP PGDN to organize");
				message_screen->visible = true;
				message_screen->locked = true;
				LoadFurniture(furniture_menu->getCurrentId(), "furniture");
				furniture_menu->disableMenu();
			}
			else if (furniture_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				furniture_menu->setViewPage(furniture_menu->current_page - 1, renderer);
				furniture_menu->marker_visible = false;
				furniture_menu->controls[3].setEnable(false);
			}
			else if (furniture_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				furniture_menu->setViewPage(furniture_menu->current_page + 1, renderer);
				furniture_menu->marker_visible = false;
				furniture_menu->controls[3].setEnable(false);
			}
			//item selector
			for (widgets::button_container::iterator it = furniture_menu->slots.begin(); it != furniture_menu->slots.end(); ++it){
				if (it->IsIn(x, y)){
					if (it->getEnable()){
						furniture_menu->setCurrentId(it->getId());
						furniture_menu->marker_rect.x = it->getX();
						furniture_menu->marker_rect.y = it->getY();
						furniture_menu->marker_visible = true;
						furniture_menu->controls[3].setEnable(true);
					}
				}
			}
		}
		/*
		else if (status == Game::ACCESSORY_MENU){
			if (accessory_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				accessory_menu->disableMenu();
			}
			if (accessory_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				return_button->setEnable(true);
				return_button->setVisible(true);
				message_screen->addMessage("Click to pick. DEL to delete. PGUP PGDN to organize");
				message_screen->visible = true;
				message_screen->locked = true;
				LoadAccessory(accessory_menu->getCurrentId(), megu_list.begin()->first,0,0,300,300, "accessory");
				SetMeguPos(1, 192, 192, SDL_FLIP_NONE);
				SetMeguDestination(1, 192, 192);
				OnRender();
				status = Game::ACCESSORY_MODE;
				accessory_menu->disableMenu();
			}
			else if (accessory_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				accessory_menu->setViewPage(accessory_menu->current_page - 1, renderer);
				accessory_menu->marker_visible = false;
				accessory_menu->controls[3].setEnable(false);
			}
			else if (accessory_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				accessory_menu->setViewPage(accessory_menu->current_page + 1, renderer);
				accessory_menu->marker_visible = false;
				accessory_menu->controls[3].setEnable(false);
			}
			//item selector
			for (widgets::button_container::iterator it = accessory_menu->slots.begin(); it != accessory_menu->slots.end(); ++it){
				if (it->IsIn(x, y)){
					if (it->getEnable()){
						accessory_menu->setCurrentId(it->getId());
						accessory_menu->marker_rect.x = it->getX();
						accessory_menu->marker_rect.y = it->getY();
						accessory_menu->marker_visible = true;
						accessory_menu->controls[3].setEnable(true);
					}
				}
			}
		}
		*/
		else if (status == Game::FOOD_MENU){
			if (food_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				food_menu->marker_visible = false;
				food_menu->disableMenu();
			}
			else if (food_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::FEEDING;
				sounds.playSFX("changed", 5);
				food_mode(food_menu->getCurrentId());
				food_menu->marker_visible = false;
				food_menu->disableMenu();

			}
			else if (food_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				food_menu->setViewPage(food_menu->current_page - 1, renderer);
				food_menu->marker_visible = false;
				food_menu->controls[3].setEnable(false);
			}
			else if (food_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				food_menu->setViewPage(food_menu->current_page + 1, renderer);
				food_menu->marker_visible = false;
				food_menu->controls[3].setEnable(false);
			}
			if (food_menu->controls[4].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::EDIT_MODE;
				return_button->setEnable(true);
				return_button->setVisible(true);
				message_screen->addMessage("Click to pick. DEL to delete. PGUP PGDN to organize");
				message_screen->visible = true;
				message_screen->locked = true;
				LoadFurniture(food_menu->getCurrentId(), "food");
				food_menu->disableMenu();
			}
			else{
				std::string class_button = food_menu->isInDisplay(x, y);
				if (class_button.compare("NONE") != 0){
					food_menu->setClassificationType(food_menu->isInDisplay(x, y), renderer);
					food_menu->setViewPage(0, renderer);
					food_menu->marker_visible = false;
					food_menu->controls[3].setEnable(false);
					sounds.playSFX("click", 5);
				}
				//item selector
				for (widgets::button_container::iterator it = food_menu->slots.begin(); it != food_menu->slots.end(); ++it){
					if (it->IsIn(x, y)){
						if (it->getEnable()){
							food_menu->setCurrentId(it->getId());
							food_menu->marker_rect.x = it->getX();
							food_menu->marker_rect.y = it->getY();
							food_menu->marker_visible = true;
							food_menu->controls[3].setEnable(true);

						}

					}
				}
			}
			//food
		}

		else if (status == Game::LAYOUT_SAVE_MENU){
			if (layout_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				layout_menu->marker_visible = false;
				layout_menu->disableMenu();
			}
			else if (layout_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("changed", 5);
				layout_menu->marker_visible = false;
				layout_menu->disableMenu();
				OnRender();
				std::stringstream ss;
				ss << std::setw(3) << std::setfill('0') << layout_menu->getCurrentId();
				std::string s = ss.str();
				serializeHouse(s);
				takeScreenshot("gamedata/house/preview/SAV" + s, 120, 120);
				status = Game::RUNNING;

			}
			else if (layout_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				layout_menu->setViewPage(layout_menu->current_page - 1, renderer);
				layout_menu->marker_visible = false;
				layout_menu->controls[3].setEnable(false);
			}
			else if (layout_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				layout_menu->setViewPage(layout_menu->current_page + 1, renderer);
				layout_menu->marker_visible = false;
				layout_menu->controls[3].setEnable(false);
			}
			else{
				//item selector
				for (widgets::button_container::iterator it = layout_menu->slots.begin(); it != layout_menu->slots.end(); ++it){
					if (it->IsIn(x, y)){
						layout_menu->setCurrentId(it->getId());
						layout_menu->marker_rect.x = it->getX();
						layout_menu->marker_rect.y = it->getY();
						layout_menu->marker_visible = true;
						layout_menu->controls[3].setEnable(true);
					}
				}
			}			//food
		}

		else if (status == Game::LAYOUT_LOAD_MENU){

			if (layout_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				layout_menu->marker_visible = false;
				layout_menu->disableMenu();
			}
			else if (layout_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);;
				sounds.playSFX("changed", 5);
				layout_menu->marker_visible = false;
				std::stringstream ss;
				ss << std::setw(3) << std::setfill('0') << layout_menu->getCurrentId();
				std::string s = ss.str();
				std::cerr << "string: " << s << std::endl;
				deserializeHouse(s);
				serializeHouse("DEFAULT");//update default
				layout_menu->disableMenu();
				status = Game::RUNNING;
			}

			else if (layout_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				layout_menu->setViewPage(layout_menu->current_page - 1, renderer);
				layout_menu->marker_visible = false;
				layout_menu->controls[3].setEnable(false);
			}
			else if (layout_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				layout_menu->setViewPage(layout_menu->current_page + 1, renderer);
				layout_menu->marker_visible = false;
				layout_menu->controls[3].setEnable(false);
			}
			else{
				//item selector
				for (widgets::button_container::iterator it = layout_menu->slots.begin(); it != layout_menu->slots.end(); ++it){
					if (it->IsIn(x, y)){
						layout_menu->setCurrentId(it->getId());
						layout_menu->marker_rect.x = it->getX();
						layout_menu->marker_rect.y = it->getY();
						layout_menu->marker_visible = true;
						layout_menu->controls[3].setEnable(true);
					}
				}
			}
		}

		else if (status == Game::FRIEND_INVITE_MENU){
			if (friends_menu->controls[2].IsIn(x, y)){
				sounds.playSFX("click", 5);
				status = Game::RUNNING;
				friends_menu->marker_visible = false;
				friends_menu->disableMenu();
			}
			else if (friends_menu->controls[3].IsIn(x, y)){
				sounds.playSFX("click", 5);;
				sounds.playSFX("changed", 5);
				friends_menu->marker_visible = false;
				deserializePlayer(friends_menu->getCurrentPath(), -1);
				friends_menu->disableMenu();
				status = Game::RUNNING;

			}
			else if (friends_menu->controls[1].IsIn(x, y)){
				sounds.playSFX("click", 5);
				friends_menu->setViewPage(friends_menu->current_page - 1, renderer);
				friends_menu->marker_visible = false;
				friends_menu->controls[3].setEnable(false);
			}
			else if (friends_menu->controls[0].IsIn(x, y)){
				sounds.playSFX("click", 5);
				friends_menu->setViewPage(friends_menu->current_page + 1, renderer);
				friends_menu->marker_visible = false;
				friends_menu->controls[3].setEnable(false);
			}
			else{
				//item selector
				for (widgets::button_container::iterator it = friends_menu->slots.begin(); it != friends_menu->slots.end(); ++it){
					if (it->IsIn(x, y)){
						if (it->getEnable()){
							std::cerr << it->getEnable() << std::endl;
							friends_menu->setCurrentId(it->getId());
							friends_menu->setCurrentString(it - friends_menu->slots.begin());
							friends_menu->marker_rect.x = it->getX();
							friends_menu->marker_rect.y = it->getY();
							friends_menu->marker_visible = true;
							friends_menu->controls[3].setEnable(true);

						}

					}
				}
			}
			//food
		}
		else if (status == Game::EDIT_MODE){
			//return button
			if (return_button->IsIn(x, y)){
				if (return_button->getEnable()){
					sounds.playSFX("click", 5);
					status = Game::RUNNING;
					return_button->setEnable(false);
					return_button->setVisible(false);
					message_screen->visible = false;
					message_screen->locked = false;
					OnRender();
					serializeHouse("DEFAULT");

				}
			}
			if (edit_holding == false){
				for (unsigned int i = 0; i < objects_list.size(); ++i){
					if (objects_list[i].IsIn(x, y)){
						object_index = i;
						edit_holding = true;
						SDL_ShowCursor(0);
					}
				}
			}
			else if (edit_holding == true){
				SDL_ShowCursor(1);
				edit_holding = false;
			}
		}

/*		else if (status == Game::ACCESSORY_MODE){
			//return button
			if (return_button->IsIn(x, y)){
				if (return_button->getEnable()){
					//change to relative values
					for (std::vector<Megu::scene_objects>::iterator i = megu_list.begin()->second.accessories_list.begin(); i != megu_list.begin()->second.accessories_list.end(); ++i){
						i->offset.x = i->rectangle.x - megu_list.begin()->second.position.x;
						i->offset.y = i->rectangle.y - megu_list.begin()->second.position.y;
						i->offset_side = i->side;
					}
					sounds.playSFX("click", 5);
					
					return_button->setEnable(false);
					return_button->setVisible(false);
					message_screen->visible = false;
					message_screen->locked = false;
					serializePlayer("gamedata/friends/player.megu", megu_list.begin()->second);
					status = Game::RUNNING;
				}
			}
			if (edit_holding == false){
				if (!megu_list.begin()->second.accessories_list.empty()){
					for (unsigned int i = 0; i < megu_list.begin()->second.accessories_list.size(); ++i){
						if (megu_list.begin()->second.accessories_list[i].IsIn(x, y)){
							megu_list.begin()->second.accessory_index = i;
							edit_holding = true;
							SDL_ShowCursor(0);
						}
					}
				}
			}
			else if (edit_holding == true){
				SDL_ShowCursor(1);
				edit_holding = false;
			}
			else{
				//do nothing
			}
		}*/
		break;

	case SDL_MOUSEMOTION:
		if (status == Game::EDIT_MODE){
			if (edit_holding == true){
				objects_list[object_index].rectangle.x = x - objects_list[object_index].rectangle.w / 2;
				objects_list[object_index].rectangle.y = y - objects_list[object_index].rectangle.h / 2;

				if (objects_list[object_index].rectangle.y > clickable_area.y + clickable_area.h)//boundary
					objects_list[object_index].rectangle.y = clickable_area.y + clickable_area.h;

			}
		}
		/*
		else if (status == Game::ACCESSORY_MODE){
			if (edit_holding == true){

				megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.x = x - megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.w / 2;
				megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.y = y - megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.h / 2;

				if (megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.y > clickable_area.y + clickable_area.h)//boundary
					megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].rectangle.y = clickable_area.y + clickable_area.h;
			}
		}*/
		else{
			for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator sprite_it = megu_list.begin(); sprite_it != megu_list.end(); ++sprite_it){
				if (sprite_it->second.pet_flag == true){
					++sprite_it->second.pet_timer;
				}
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator sprite_it = megu_list.begin(); sprite_it != megu_list.end(); ++sprite_it){
			sprite_it->second.pet_flag = false;
			sprite_it->second.pet_timer = 0;
		}
		break;
	case SDL_KEYDOWN:
		if (status == Game::EDIT_MODE){
			if (edit_holding == true){
				if (e->key.keysym.scancode == SDL_SCANCODE_DELETE){
					objects_list.erase(objects_list.begin() + object_index);
					object_index = -1;
					edit_holding = false;
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_BACKSPACE){

					if (objects_list[object_index].side == SDL_FLIP_HORIZONTAL)
						objects_list[object_index].side = SDL_FLIP_NONE;
					else if (objects_list[object_index].side == SDL_FLIP_NONE)
						objects_list[object_index].side = SDL_FLIP_HORIZONTAL;
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_PAGEDOWN){

					if (objects_list.size() != 1){
						if (object_index != 0){

							--objects_list[object_index].render_id;
							++objects_list[object_index - 1].render_id;
							std::sort(objects_list.begin(), objects_list.end(), sortobjects);
							--object_index;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_PAGEUP){
					if (objects_list.size() != 1){
						if (object_index != objects_list.size() - 1){
							++objects_list[object_index].render_id;
							--objects_list[object_index + 1].render_id;
							std::sort(objects_list.begin(), objects_list.end(), sortobjects);
							++object_index;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_END){
					if (objects_list.size() != 1){
						if (object_index != 0){

							for (unsigned int i = 0; i < object_index; ++i){
								++objects_list[i].render_id;//remove gap
							}
							objects_list[object_index].render_id = 1;
							std::sort(objects_list.begin(), objects_list.end(), sortobjects);
							object_index = 0;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_HOME){
					if (objects_list.size() != 1){
						if (object_index != objects_list.size() - 1){
							for (unsigned int i = object_index + 1; i < objects_list.size(); ++i){
								--objects_list[i].render_id;//remove gap
							}
							objects_list[object_index].render_id = objects_list.size();
							std::sort(objects_list.begin(), objects_list.end(), sortobjects);
							object_index = objects_list.size() - 1;
						}
					}
				}
			}
		}

/*		else if (status == Game::ACCESSORY_MODE){
			if (edit_holding == true){
				if (e->key.keysym.scancode == SDL_SCANCODE_DELETE){
					megu_list.begin()->second.accessories_list.erase(megu_list.begin()->second.accessories_list.begin() + megu_list.begin()->second.accessory_index);
					megu_list.begin()->second.accessory_index = -1;
					edit_holding = false;
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_BACKSPACE){

					if (megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].side == SDL_FLIP_HORIZONTAL)
						megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].side = SDL_FLIP_NONE;
					else if (megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].side == SDL_FLIP_NONE)
						megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].side = SDL_FLIP_HORIZONTAL;
				}

				if (e->key.keysym.scancode == SDL_SCANCODE_PAGEDOWN){

					if (megu_list.begin()->second.accessories_list.size() != 1){
						if (megu_list.begin()->second.accessory_index != 0){

							--megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].render_id;
							++megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index - 1].render_id;
							std::sort(megu_list.begin()->second.accessories_list.begin(), megu_list.begin()->second.accessories_list.end(), sortobjects);
							--megu_list.begin()->second.accessory_index;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_PAGEUP){
					if (megu_list.begin()->second.accessories_list.size() != 1){
						if (megu_list.begin()->second.accessory_index != megu_list.begin()->second.accessories_list.size() - 1){
							++megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].render_id;
							--megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index + 1].render_id;
							std::sort(megu_list.begin()->second.accessories_list.begin(), megu_list.begin()->second.accessories_list.end(), sortobjects);
							++megu_list.begin()->second.accessory_index;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_END){
					if (megu_list.begin()->second.accessories_list.size() != 1){
						if (megu_list.begin()->second.accessory_index != 0){
							for (unsigned int i = 0; i < megu_list.begin()->second.accessory_index; ++i){
								++megu_list.begin()->second.accessories_list[i].render_id;//remove gap
							}
							megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].render_id = 1;
							std::sort(megu_list.begin()->second.accessories_list.begin(), megu_list.begin()->second.accessories_list.end(), sortobjects);
							megu_list.begin()->second.accessory_index = 0;
						}
					}
				}
				if (e->key.keysym.scancode == SDL_SCANCODE_HOME){
					if (megu_list.begin()->second.accessories_list.size() != 1){
						if (megu_list.begin()->second.accessory_index != megu_list.begin()->second.accessories_list.size() - 1){
							for (unsigned int i = megu_list.begin()->second.accessory_index + 1; i < megu_list.begin()->second.accessories_list.size(); ++i){
								--megu_list.begin()->second.accessories_list[i].render_id;//remove gap
							}
							megu_list.begin()->second.accessories_list[megu_list.begin()->second.accessory_index].render_id = megu_list.begin()->second.accessories_list.size();
							std::sort(megu_list.begin()->second.accessories_list.begin(), megu_list.begin()->second.accessories_list.end(), sortobjects);
							megu_list.begin()->second.accessory_index = megu_list.begin()->second.accessories_list.size() - 1;
						}
					}
				}
			}
		}*/
		else{
			if (status == Game::RUNNING){
				if (e->key.keysym.scancode == SDL_SCANCODE_TAB){
					takeScreenshot("img", 640, 640);
				}
			}
		}
		break;
	}
}

void MeguEngine::foodAnimationCallback(const PLAYER_ID &player){
	megu_list[player].blink = Megu::NONE;
	megu_list[player].state = Megu::VERY_HAPPY;
	int last_time = 0;
	int current_time = 0;
	int diff_time = 0;
	int animation_delay = 1000;
	sounds.playSFX("food", 5);
	while (diff_time < animation_delay){
		curr_time = SDL_GetTicks();
		diff_time += curr_time - last_time;
		last_time = curr_time;
	}

	switch (megu_list[player].size){
	case 1:
	{
			  sounds.playSFX("happy1", player);
			  break;
	}
	case 2:
	{
			  sounds.playSFX("happy2", player);
			  break;
	}
	case 3:
	{
			  sounds.playSFX("happy3", player);
			  break;
	}
	}
}

void MeguEngine::food_mode(const int &ID){
	int nplayers = megu_list.size();
	kit.food.push_back(*library.food_lib[ID].texture.begin());
	kit.visible = true;
	for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator it = megu_list.begin(); it != megu_list.end(); ++it){
		it->second.state = Megu::WALK_ONE;
		it->second.blink = Megu::NONE;

		it->second.run_flag = true;
		if (it->first % 2){
			SetMeguDestination(it->first, kit.rect_t.x + kit.even_megu.x, kit.rect_t.y + kit.even_megu.y);
		}
		else{
			SetMeguDestination(it->first, kit.rect_t.x + kit.odd_megu.y, kit.rect_t.y + kit.odd_megu.y);
		}
	}
}

bool MeguEngine::updateBackground(const int  &id){
	house_id = id;
	if (house != NULL){
		SDL_DestroyTexture(house);
	}
	std::string path("assets/Houses/ID" + std::to_string(house_id) + "@2x.png");
	house = loadTexture(path);
	if (house == NULL){
		std::cerr << "Error Loading Background" << std::endl;
		return false;
	}
	return true;
}

SDL_Texture* MeguEngine::loadTexture(std::string path)
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

void MeguEngine::LoadMegu(const int &new_megu_id, const PLAYER_ID &player){
	if (megu_list.find(player) != megu_list.end()){
		for (int i = 0; i < 11; ++i)
			SDL_DestroyTexture(megu_list[player].sprites[i]);
	}
	std::string suffix;
	if (mini){
		suffix = ".png";
	}
	else{
		suffix = "@2x.png";
	}
		for (int i = 0; i < 11; ++i){
			megu_list[player].sprites[i] = loadTexture("assets/megu/ID" + std::to_string(new_megu_id) + "_" + std::to_string(i + 3) + suffix);
		}
		std::string path = "assets//megu//ID" + std::to_string(new_megu_id) + "_" + std::to_string(3) + suffix;
		std::cerr << path << std::endl;
	megu_list[player].pet_race = library.megu_lib[new_megu_id].en_name;
	megu_list[player].id = new_megu_id;
	megu_list[player].shadow = loadTexture(shadow_texture);
	megu_list[player].size = library.megu_lib[new_megu_id].size;
	megu_list[player].position.x = megu_size.x;
	megu_list[player].position.y = megu_size.y;
	megu_list[player].destination.x = megu_list[player].position.x;
	megu_list[player].destination.y = megu_list[player].position.y;
	megu_list[player].position.h = megu_size.h;
	megu_list[player].position.w = megu_size.w;
	megu_list[player].side = SDL_FLIP_NONE;
	megu_list[player].state = Megu::STOP_ONE;
	megu_list[player].blink = Megu::NONE;
	SDL_SetTextureAlphaMod(megu_list[player].shadow, 46);
	megu_list[player].shadow_pos.x = megu_list[player].position.x + shadow_pos.x;
	megu_list[player].shadow_pos.y = megu_list[player].position.y + shadow_pos.y;
	megu_list[player].shadow_pos.h = shadow_pos.h;
	megu_list[player].shadow_pos.w = shadow_pos.w;

	
}
void MeguEngine::SetMeguPos(const PLAYER_ID &player, const int &x, const int &y, const SDL_RendererFlip &side){
	megu_list[player].position.x = x;
	megu_list[player].position.y = y;
	megu_list[player].side = side;
	megu_list[player].shadow_pos.x = x + shadow_pos.x;// +shadow_pos.x;
	megu_list[player].shadow_pos.y = y + shadow_pos.y;// + shadow_pos.y;
	megu_list[player].shadow_pos.h = shadow_pos.h;
	megu_list[player].shadow_pos.w = shadow_pos.w;
	
		
}

void MeguEngine::SetMeguState(const PLAYER_ID &player, const Megu::MEGU_STATE &new_state, const Megu::MEGU_EYE &eye_state){
	megu_list[player].state = new_state;
	megu_list[player].blink = eye_state;
}

void MeguEngine::OnEvent(SDL_Event* Event) {
	handleMouseEvent(Event);
	if (Event->type == SDL_QUIT){
		Running = false;
	}
}

void MeguEngine::getConstrainedMovement(const int &wx, const int &wy, int &out_wx, int &out_wy){
	if (wy > clickable_area.h + clickable_area.y - megu_size.h)
		out_wy = clickable_area.h + clickable_area.y - megu_size.h;
	else if (wy < clickable_area.y - megu_size.h)
		out_wy = clickable_area.y;
	else{
		out_wy = wy;
	}
	if (wx < clickable_area.x + megu_size.w/2)
		out_wx = clickable_area.x;
	else if (wx > clickable_area.x + clickable_area.w - megu_size.w/2)
		out_wx = clickable_area.x + clickable_area.w - megu_size.w/2;
	else{
		out_wx = wx;
	}
}

void MeguEngine::MeguStateMachine(){
	//get timers
	if (status == Game::RUNNING || status == Game::FEEDING){
	curr_time = SDL_GetTicks();
	uint32_t diff_time = curr_time - last_time;//calculate diff
	msg_time += diff_time;
	for (std::vector<Megu::scene_objects>::iterator obj_it = objects_list.begin(); obj_it != objects_list.end(); ++obj_it){
		if (obj_it->animation_order.size() > 0){//if animated
			obj_it->current_time += diff_time; //increment time
			if (obj_it->current_time > obj_it->animation_interval){ //if transitioning texture
				obj_it->current_time = 0; //zero timer
				++obj_it->current_frame; //move to next frame
				if (obj_it->current_frame > obj_it->animation_order.size()){//return to begin
					obj_it->current_frame = 0;
				
				}
			}
		}
	}
	
		for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator sprite_it = megu_list.begin(); sprite_it != megu_list.end(); ++sprite_it){
			std::uniform_int_distribution<int> distribution(1, 36);
			msg_time += diff_time;
			sprite_it->second.blink_timer += diff_time;
			sprite_it->second.animation_timer += diff_time;
			sprite_it->second.random_action_timer += diff_time + distribution(sprite_it->second.generator);
			//petting flag on, hovering
			if (sprite_it->second.pet_flag){
				if (sprite_it->second.pet_timer > megu_pet_counter){
					sprite_it->second.pet_timer = 0;
					if (sprite_it->second.pet_counting < 8){
						sprite_it->second.state = Megu::HAPPY;
						++sprite_it->second.pet_counting;
						sprite_it->second.run_flag = false;
						sounds.playSFX("pet", sprite_it->first);
						if (sprite_it->second.pet_counting % 3 == 0){
							std::uniform_int_distribution<int> pet_distrib(0, 11);
							std::string msg = sprite_it->second.pet_name + pet_message_list[pet_distrib(sprite_it->second.generator)];
							message_screen->addMessage(msg);
							message_screen->visible = true;
							message_screen->setTimeOut(3000);
							message_screen->setTimer(0);
						}
					}
					else{
						sprite_it->second.state = Megu::VERY_HAPPY;
						sprite_it->second.pet_counting = 0;
						switch (sprite_it->second.size){
						case 1:
						{
								  sounds.playSFX("happy1", sprite_it->first);
								  break;
						}
						case 2:
						{
								  sounds.playSFX("happy2", sprite_it->first);
								  break;
						}
						case 3:
						{
								  sounds.playSFX("happy3", sprite_it->first);
								  break;
						}
						}
						std::uniform_int_distribution<int> pet_distrib(0, 11);
						std::string msg = sprite_it->second.pet_name + pet_message_list[pet_distrib(sprite_it->second.generator)];
						message_screen->addMessage(msg);
						message_screen->visible = true;
						message_screen->setTimeOut(5000);
						message_screen->setTimer(0);
					}

					sprite_it->second.blink = Megu::NONE;
					sprite_it->second.animation_timer = 0;
					sprite_it->second.misc_timer = 0;
					sprite_it->second.destination.x = sprite_it->second.position.x;
					sprite_it->second.destination.y = sprite_it->second.position.y;
				}
			}
			//blink eye trigger
			if (sprite_it->second.state == Megu::STOP_ONE ||
				sprite_it->second.state == Megu::STOP_TWO){
				if (sprite_it->second.blink_timer > megu_blink_counter){
					sprite_it->second.blink_timer -= megu_blink_counter;
					sprite_it->second.blink = Megu::BLINK;
					sprite_it->second.blink_openeye = 0;
				}
				//open eye blink
				if (sprite_it->second.blink == Megu::BLINK){
					sprite_it->second.blink_openeye += curr_time - last_time;
					uint32_t diff_time = curr_time - last_time;
					if (sprite_it->second.blink_openeye > megu_openeye_counter){
						sprite_it->second.blink_openeye = 0;
						sprite_it->second.blink = Megu::OPEN;
					}
				}
			}
			//animation switching
			unsigned int animation_time = megu_animation_counter;
			if (sprite_it->second.run_flag){
				animation_time /= 8;
			}
			//animation switching
			if (sprite_it->second.animation_timer > animation_time)
			{
				sprite_it->second.animation_timer = 0;
				switch (sprite_it->second.state){
				case Megu::WALK_ONE:
					if (sprite_it->second.position.x == sprite_it->second.destination.x && //get to destination, end animation cycle
						sprite_it->second.position.y == sprite_it->second.destination.y){
						sprite_it->second.run_flag = false;
						sprite_it->second.state = Megu::STOP_ONE;
						sprite_it->second.blink = Megu::OPEN;
						sprite_it->second.blink_timer = 0;
					}
					else{
						sprite_it->second.state = Megu::WALK_TWO;
						MeguPositionHandler(sprite_it->first);
					}
					break;
				case Megu::WALK_TWO:
					if (sprite_it->second.position.x == sprite_it->second.destination.x && //get to destination, end animation cycle
						sprite_it->second.position.y == sprite_it->second.destination.y){
						sprite_it->second.run_flag = false;
						sprite_it->second.state = Megu::STOP_ONE;
						sprite_it->second.blink = Megu::OPEN;
						sprite_it->second.blink_timer = 0;
					}
					else{
						sprite_it->second.state = Megu::WALK_ONE;
						MeguPositionHandler(sprite_it->first);
					}
					break;
				case Megu::STOP_ONE:

					sprite_it->second.state = Megu::STOP_TWO;
					break;

				case Megu::STOP_TWO:
					//blink eye trigger
					if (sprite_it->second.blink_timer > megu_blink_counter){
						sprite_it->second.blink_timer -= megu_blink_counter;
						sprite_it->second.blink = Megu::BLINK;
						sprite_it->second.blink_openeye = 0;
					}
					//open eye blink
					if (sprite_it->second.blink == Megu::BLINK){
						sprite_it->second.blink_openeye += curr_time - last_time;
						uint32_t diff_time = curr_time - last_time;
						if (sprite_it->second.blink_openeye > megu_openeye_counter){
							sprite_it->second.blink_openeye = 0;
							sprite_it->second.blink = Megu::OPEN;
						}
					}
					sprite_it->second.state = Megu::STOP_ONE;
					break;
				}
			}
			if (sprite_it->second.state == Megu::AFRAID ||
				sprite_it->second.state == Megu::HAPPY ||
				sprite_it->second.state == Megu::SCARED ||
				sprite_it->second.state == Megu::THINKING
				){
				sprite_it->second.misc_timer += diff_time;
				if (sprite_it->second.misc_timer > megu_misc_counter){
					sprite_it->second.misc_timer = 0;
					sprite_it->second.state = Megu::STOP_ONE;
					sprite_it->second.blink = Megu::OPEN;
				}
			}
			if (sprite_it->second.state == Megu::VERY_HAPPY){
				sprite_it->second.misc_timer += diff_time;
				if (sprite_it->second.misc_timer > megu_misc_counter * 2){
					sprite_it->second.misc_timer = 0;
					sprite_it->second.state = Megu::STOP_ONE;
					sprite_it->second.blink = Megu::OPEN;
				}
			}

			if (status == Game::FEEDING){
				if (sprite_it->second.state == Megu::STOP_ONE || sprite_it->second.state == Megu::STOP_TWO){
					foodAnimationCallback(sprite_it->first);
					++player_counter;
					if (player_counter == megu_list.size()){
						player_counter = 0;
						status = Game::RUNNING;
						kit.destroy_food_kit();
						sounds.playSFX("food", 5);

					}
				}
			}
			if (sprite_it->second.random_action_timer > megu_event_action){
				sprite_it->second.random_action_timer -= megu_event_action;
				sprite_it->second.random_action_timer += distribution(sprite_it->second.generator) * 500;
				if (sprite_it->second.state == Megu::STOP_ONE || sprite_it->second.state == Megu::STOP_TWO){
					//random events
					int w_x = 0;
					int w_y = 0;
					switch (distribution(sprite_it->second.generator)){
					case 1:
					{
							  sprite_it->second.state = Megu::HAPPY;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 2:
					{
							  sprite_it->second.state = Megu::HAPPY;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 3:
					{
							  sprite_it->second.state = Megu::THINKING;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 4:
					{
							  sprite_it->second.state = Megu::HAPPY;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = true;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 5:
					{
							  sprite_it->second.state = Megu::HAPPY;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 6:
					{
							  sprite_it->second.state = Megu::VERY_HAPPY;
							  sprite_it->second.blink = Megu::NONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 7:
					{
							  sprite_it->second.state = Megu::STOP_ONE;
							  sprite_it->second.blink = Megu::OPEN;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 8:
					{
							  sprite_it->second.blink = Megu::OPEN;
							  sprite_it->second.state = Megu::STOP_ONE;
							  sprite_it->second.run_flag = false;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 9:
					{
							  sprite_it->second.state = Megu::STOP_ONE;
							  sprite_it->second.blink = Megu::OPEN;
							  sprite_it->second.run_flag = true;
							  sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							  break;
					}
					case 10:
					{
							   sprite_it->second.state = Megu::STOP_ONE;
							   sprite_it->second.blink = Megu::OPEN;
							   sprite_it->second.run_flag = true;
							   sounds.playSFX("megu" + std::to_string(distribution(sprite_it->second.generator)), sprite_it->first);
							   break;
					}
					case 11:{
								w_x = sprite_it->second.position.x + 10 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y;

								sprite_it->second.destination.x = w_x;
								sprite_it->second.destination.y = w_y;
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 12:{
								w_x = sprite_it->second.position.x + 5 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y + 5 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 13:{
								w_x = sprite_it->second.position.x;
								w_y = sprite_it->second.position.y + 10 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 14:{
								w_x = sprite_it->second.position.x - 5 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y + 5 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 15:{
								w_x = sprite_it->second.position.x - 10 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y;
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 16:{
								w_x = sprite_it->second.position.x - 5 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y - 5 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 17:{
								w_x = sprite_it->second.position.x;
								w_y = sprite_it->second.position.y - 10 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					case 18:{
								w_x = sprite_it->second.position.x + 5 * distribution(sprite_it->second.generator);
								w_y = sprite_it->second.position.y - 5 * distribution(sprite_it->second.generator);
								getConstrainedMovement(w_x, w_y, sprite_it->second.destination.x, sprite_it->second.destination.y);
								sprite_it->second.blink = Megu::NONE;
								sprite_it->second.state = Megu::WALK_ONE;
								break;
					}
					}
				}
			}
		}
		last_time = curr_time;
		//message timer
		if (!message_screen->locked){
			if (message_screen->visible){
				if (message_screen->getTimer() < message_screen->getTimeout()){
					message_screen->setTimer(message_screen->getTimer() + diff_time);
				}
				else if (message_screen->getTimer() >= message_screen->getTimeout()){
					message_screen->visible = false;
					message_screen->setTimer(0);
					message_screen->setTimeOut(0);
				}
			}
			if (msg_time > megu_msg_counter){
				msg_time = 0;
				std::uniform_int_distribution<int> distribution(0, random_message_list.size()-1);
				message_screen->addMessage(random_message_list[distribution(megu_list.begin()->second.generator)]);
				message_screen->setTimeOut(5000);
				message_screen->setTimer(0);
				message_screen->visible = true;
			}
		}
	}
}
void MeguEngine::OnRender() {
	mutex = SDL_CreateMutex();
	if (SDL_LockMutex(mutex) == 0){
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, house, 0, &menu_background_pos);
		for (std::vector<Megu::scene_objects>::iterator obj_it = objects_list.begin(); obj_it != objects_list.end(); ++obj_it){
			SDL_RenderCopyEx(renderer, obj_it->textures.at(obj_it->current_frame), NULL, &obj_it->rectangle, 0, 0, obj_it->side);
		}
	/*	if (status == Game::ACCESSORY_MODE){
			SDL_RenderCopyEx(renderer, megu_list.begin()->second.sprites[2], NULL, &megu_list.begin()->second.position, 0, 0, megu_list.begin()->second.side);
			SDL_RenderCopyEx(renderer, megu_list.begin()->second.sprites[4], NULL, &megu_list.begin()->second.position, 0, 0, megu_list.begin()->second.side);
			for (std::vector<Megu::scene_objects>::iterator obj_it = megu_list.begin()->second.accessories_list.begin(); obj_it != megu_list.begin()->second.accessories_list.end(); ++obj_it){
				SDL_RenderCopyEx(renderer, obj_it->textures.at(0), NULL, &obj_it->rectangle, 0, 0, obj_it->side);
			}
		}*/
		 if (status == Game::EDIT_MODE && status == Game::NAME_MENU){
		}
		else if (status == Game::RUNNING || status == Game::FEEDING){
			for (std::vector<render_ordering>::iterator player_it = render_order.begin(); player_it != render_order.end(); ++player_it){
				SDL_RenderCopy(renderer, megu_list[player_it->player].shadow, NULL, &megu_list[player_it->player].shadow_pos);
				SDL_RenderCopyEx(renderer, megu_list[player_it->player].sprites[megu_list[player_it->player].state], NULL, &megu_list[player_it->player].position, 0, 0, megu_list[player_it->player].side);
				if (megu_list[player_it->player].blink != Megu::NONE){
					SDL_RenderCopyEx(renderer, megu_list[player_it->player].sprites[megu_list[player_it->player].blink], NULL, &megu_list[player_it->player].position, 0, 0, megu_list[player_it->player].side);
				}
				for (std::vector<Megu::scene_objects>::iterator obj_it = megu_list[player_it->player].accessories_list.begin(); obj_it != megu_list[player_it->player].accessories_list.end(); ++obj_it){
					obj_it->rectangle.x = megu_list[player_it->player].position.x;
					if (megu_list[player_it->player].side == SDL_FLIP_NONE){
						obj_it->rectangle.x += obj_it->offset.x;
						if (obj_it->offset_side == SDL_FLIP_NONE){
							obj_it->side = SDL_FLIP_NONE;
						}
						else{
							obj_it->side = SDL_FLIP_HORIZONTAL;
						}
					}
					else{
						obj_it->rectangle.x += (megu_list[player_it->player].position.w - obj_it->offset.x - obj_it->rectangle.w);
						if (obj_it->offset_side == SDL_FLIP_HORIZONTAL){
							obj_it->side = SDL_FLIP_NONE;
						}
						else{
							obj_it->side = SDL_FLIP_HORIZONTAL;
						}
					}
					obj_it->rectangle.y = obj_it->offset.y + megu_list[player_it->player].position.y;
					SDL_RenderCopyEx(renderer, obj_it->textures.at(0), NULL, &obj_it->rectangle, 0, 0, obj_it->side);
				}
			}
		}
		if (kit.visible){
			SDL_RenderCopy(renderer, kit.table_shadow, NULL, &kit.rect_s);
			SDL_RenderCopy(renderer, kit.table, NULL, &kit.rect_t);
			for (int i = 0; i < kit.food.size(); ++i){
				SDL_RenderCopy(renderer, kit.food.at(i), NULL, &kit.rect_f);
			}
		}
		SDL_RenderCopy(renderer, menu_bar, 0, &menu_bar_pos);
		for (std::vector<TtfWrapper>::iterator it = ui_texts.begin(); it != ui_texts.end(); ++it){
			it->Render(renderer);
		}
		return_button->Render(renderer);
		care_button->Render(renderer);
		edit_button->Render(renderer);
		option_button->Render(renderer);
		food_menu->Render(renderer);
		food_menu->RenderButtons(renderer);
		furniture_menu->Render(renderer);
		furniture_menu->RenderButtons(renderer);
		pet_menu->Render(renderer);
		pet_menu->RenderButtons(renderer);
		house_menu->Render(renderer);
		house_menu->RenderButtons(renderer);
		friends_menu->Render(renderer);
		friends_menu->RenderButtons(renderer);
		layout_menu->Render(renderer);
		layout_menu->RenderButtons(renderer);
		care_menu->Render(renderer);
		edit_menu->Render(renderer);
		name_menu->Render(renderer);
		options_menu->Render(renderer);
		message_screen->Render(renderer);
	//	accessory_menu->Render(renderer);

		for (std::map<PLAYER_ID, Megu::megu_sprites>::iterator it = megu_list.begin(); it != megu_list.end(); ++it){
			if (it->second.pet_flag){
				SDL_ShowCursor(0);
				SDL_Rect m_pos;
				m_pos.h = 120;
				m_pos.w = 120;
				if (mini){
					m_pos.h /= 2;
					m_pos.w /= 2;
				}
				SDL_GetMouseState(&m_pos.x, &m_pos.y);
				m_pos.x -= m_pos.w / 2;
				m_pos.y -= m_pos.h / 2;
				SDL_RenderCopy(renderer, mouse_cursor, NULL, &m_pos);
				break;
			}
			else{
				SDL_ShowCursor(1);
			}
		}
		SDL_RenderPresent(renderer);
		SDL_DestroyMutex(mutex);
	}
	else {
		std::cerr << "could not lock mutex " << std::endl;
	}

}
void MeguEngine::OnLoop() {
	//while (Running){
	MeguStateMachine();
	//	}
	//	kill_thread();
}
void MeguEngine::OnCleanup() {
	SDL_Quit();
}
void MeguEngine::SetMeguDestination(const PLAYER_ID &player, const int &x, const int &y){
	megu_list[player].destination.x = x;
	megu_list[player].destination.y = y;
}
void MeguEngine::MeguClickScene(const PLAYER_ID &player, const int &x, const int&y)
{
	SetMeguDestination(player, x, y);
	megu_list[player].state = Megu::WALK_ONE;
	megu_list[player].blink = Megu::NONE;
}
void MeguEngine::MeguPositionHandler(const PLAYER_ID &player){

	//set model side
	SDL_RendererFlip side;
	if (megu_list[player].position.x < megu_list[player].destination.x)
		side = SDL_FLIP_HORIZONTAL;
	else
		side = SDL_FLIP_NONE;
	//total distance
	double dist = sqrt(pow(megu_list[player].position.x - megu_list[player].destination.x, 2) + pow(megu_list[player].position.y - megu_list[player].destination.y, 2));
	//distance components x and y
	double addx;
	double addy;
	//calculate decomposed distance x velocity
	addx = megu_spd*(megu_list[player].destination.x - megu_list[player].position.x) / dist;
	addy = megu_spd*(megu_list[player].destination.y - megu_list[player].position.y) / dist;
	//if running
	if (megu_list[player].run_flag){
		addx *= 0.5;
		addy *= 0.5;
	}
	//add to current position
	addx += (int)megu_list[player].position.x;
	//stop criteria
	if (megu_list[player].destination.x - megu_list[player].position.x > 0){
		if (addx > megu_list[player].destination.x)
			addx = megu_list[player].destination.x;
	}
	else{
		if (addx < megu_list[player].destination.x)
			addx = megu_list[player].destination.x;
	}
	//same for y
	addy += megu_list[player].position.y;
	if (megu_list[player].destination.y - megu_list[player].position.y > 0){
		if (addy > megu_list[player].destination.y)
			addy = megu_list[player].destination.y;
	}
	else{
		if (addy < megu_list[player].destination.y)
			addy = megu_list[player].destination.y;
	}
	//update Megu Position
	SetMeguPos(player, (int)addx, (int)addy, side);
	//update rendering order
	SetRenderOrder();
}
int MeguEngine::OnExecute() {
	if (OnInit() == false) {
		return -1;
	}
    std::cerr << "loading player" << std::endl;
	std::ifstream f("gamedata/friends/player.megu");
	if (f.good()) {
		f.close();
        std::cerr << "player found" << std::endl;
		deserializePlayer("gamedata/friends/player.megu");
	}
    
	else {
		f.close();
        std::cerr << "Loading Megu default" << std::endl;
		LoadMegu(1, 1);
		megu_list[1].owner_name = "NEW OWNER";
		megu_list[1].pet_name = "MEGU";
		megu_list[1].position.x = 200;
		megu_list[1].position.y = 200;
		if (mini){
			megu_list[1].position.x /= 2;
			megu_list[1].position.y /= 2;
		}
		megu_list[1].shadow_pos.x = megu_size.x + shadow_pos.x;
		megu_list[1].shadow_pos.y = megu_size.y + shadow_pos.y;
		megu_list[1].blink = Megu::OPEN;
		megu_list[1].state = Megu::STOP_ONE;
	}
	ui_texts[0].text = megu_list[1].owner_name;
	ui_texts[1].text = megu_list[1].pet_name;
	//ui_texts[2].text = megu_list[1].pet_race;
	for (std::vector<TtfWrapper>::iterator it = ui_texts.begin(); it != ui_texts.end(); ++it){
		it->loadFromRenderedText(renderer);
	}
    std::cerr << "load house" << std::endl;
	deserializeHouse("DEFAULT");
    std::cerr << "set render order" << std::endl;
	SetRenderOrder();
	//init sound
    std::cerr << "play house bgm" << std::endl;
	sounds.playBGM("house");
	SDL_Event Event;

	//start_thread();
	while (Running) {
		OnLoop();
		while (SDL_PollEvent(&Event)) {
			OnEvent(&Event);
		}
		OnRender();
	}
	OnCleanup();

	return 0;
}
void MeguEngine::populateHouseLib(widgets::MeguMenus &menu, std::map<ID, item_library::house> &item_lib){
	for (std::map<ID, item_library::house>::iterator it = item_lib.begin(); it != item_lib.end(); ++it){
		menu.id_library["house"].push_back(it->first);
	}
}

void MeguEngine::populateLib(widgets::MeguMenus &menu, std::map<ID, item_library::item> &item_lib){
	for (std::map<ID, item_library::item>::iterator it = item_lib.begin(); it != item_lib.end(); ++it){
		menu.id_library[it->second.type].push_back(it->first);
	}
}

void MeguEngine::populatePetLib(widgets::MeguMenus &menu, std::map<ID, item_library::megu> &item_lib){
	for (std::map<ID, item_library::megu>::iterator it = item_lib.begin(); it != item_lib.end(); ++it){
		menu.id_library["megu"].push_back(it->first);
	}
}
//thread impl

void MeguEngine::LoadAccessory(const int &accessory_id, const int &player_id, const int &off_x, const int &off_y, const int &pos_x, const int &pos_y, const std::string &type, const SDL_RendererFlip &side){
	Megu::scene_objects new_obj;
	new_obj.animation_interval = 0;
	new_obj.current_frame = 0;
	new_obj.current_time = 0;
	new_obj.id = accessory_id;
	new_obj.type = type;
	std::string suffix;
	if (mini){
		suffix = ".png";
	}
	else{
		suffix = ".png";
	}
	if (type.compare("accessory") == 0){
		new_obj.textures.push_back(loadTexture("assets/Accessory/FUKU" + std::to_string(new_obj.id) + suffix));
	}
	SDL_QueryTexture(*new_obj.textures.begin(), NULL, NULL, &new_obj.rectangle.w, &new_obj.rectangle.h);
	new_obj.offset.x = off_x;
	new_obj.offset.y = off_y;
	new_obj.rectangle.x = pos_x;
	new_obj.rectangle.y = pos_y;
	new_obj.side = side;
	new_obj.offset_side = side;
	if (megu_list[player_id].accessories_list.size() == 0){
		new_obj.render_id = 1;
	}
	else{
		new_obj.render_id = megu_list[player_id].accessories_list.rend()->render_id + 1;
	}
	megu_list[player_id].accessories_list.push_back(new_obj);
}

void MeguEngine::LoadFurniture(const int &furniture_id, const std::string &type, const SDL_RendererFlip &side){
	Megu::scene_objects new_obj;
	new_obj.animation_interval = 0;
	new_obj.current_frame = 0;
	new_obj.current_time = 0;
	new_obj.id = furniture_id;
	new_obj.type = type;
	std::string suffix;
	if (mini){
		suffix = ".png";
	}
	else{
		suffix = "@2x.png";
	}
	if (type.compare("furniture") == 0){
		new_obj.textures.push_back(loadTexture("assets/Furniture/KG" + std::to_string(new_obj.id) + suffix));
		if (library.furniture_lib[furniture_id].isanimated){
			new_obj.animation_order = library.furniture_lib[furniture_id].animation_order;
			new_obj.animation_interval = library.furniture_lib[furniture_id].animation_time;
			for (std::vector<int>::iterator it = new_obj.animation_order.begin(); it != new_obj.animation_order.end(); ++it){
				std::stringstream ss;
				ss << std::setw(2) << std::setfill('0') << *it;
				std::string s = ss.str();
				new_obj.textures.push_back(loadTexture("assets/Furniture/KG" + std::to_string(new_obj.id) + "_" + s + suffix));
			}
		}

	}
	else {
		new_obj.textures.push_back(loadTexture("assets/Food/FOOD_" + std::to_string(new_obj.id) + suffix));
	}
	SDL_QueryTexture(*new_obj.textures.begin(), NULL, NULL, &new_obj.rectangle.w, &new_obj.rectangle.h);
	new_obj.rectangle.x = 200;
	new_obj.rectangle.y = 200;
	new_obj.side = side;
	if (objects_list.size() == 0){
		new_obj.render_id = 1;
	}
	else{
		new_obj.render_id = objects_list.rend()->render_id + 1;
	}
	objects_list.push_back(new_obj);
}

void MeguEngine::serializePlayer(const std::string &path, Megu::megu_sprites &player){
		std::ofstream file;
		file.open(path.c_str());
		if (file.is_open()){
			file << player.owner_name << "\r\n";
			file << player.pet_name << "\r\n";
			file << player.id << "\r\n";
			int num_objs = player.accessories_list.size();
			file << num_objs << "\r\n";
			for (int i = 0; i < num_objs; ++i){
				file << player.accessories_list[i].id << "\r\n";
				file << player.accessories_list[i].type << "\r\n";
				file << player.accessories_list[i].render_id << "\r\n";
				if (mini == false){
					file << player.accessories_list[i].offset.x << "\r\n";
					file << player.accessories_list[i].offset.y << "\r\n";
				}
				else{
					file << player.accessories_list[i].offset.x * 2 << "\r\n";
					file << player.accessories_list[i].offset.y * 2 << "\r\n";
				}
				if (player.accessories_list[i].offset_side == SDL_FLIP_NONE){
					file << 0 << "\r\n";
				}
				if (player.accessories_list[i].offset_side == SDL_FLIP_HORIZONTAL){
					file << 1 << "\r\n";
				}
			}
			file.close();
		}
		else{
			std::cerr << "error opening file" << std::endl;
		}
	}
void MeguEngine::deserializePlayer(const std::string &path, const int &id){
	mutex = SDL_CreateMutex();
	if (SDL_LockMutex(mutex) == 0){
		std::ifstream file;
		std::string line;

		file.open(path.c_str());
		int megu_id;
		std::string owner_name;
		std::string pet_name;
		int num_obj = 0;
		std::vector<Megu::scene_objects> acc_list;
		
		if (file.is_open()){
			file >> owner_name;
			file >> pet_name;
			file >> megu_id;
			file >> num_obj;
			for (int i = 0; i < num_obj; ++i){
				Megu::scene_objects obj;
				file >> obj.id;
				file >> obj.type;
				file >> obj.render_id;
				file >> obj.offset.x;
				file >> obj.offset.y;
				if (mini == true){
					obj.offset.x /= 2;
					obj.offset.y /= 2;
				}
				int side;
				file >> side;
				if (side == 0){
					obj.offset_side = SDL_FLIP_NONE;
				}
				else{
					obj.offset_side = SDL_FLIP_HORIZONTAL;
				}		
				acc_list.push_back(obj);
			}
			file.close();
			std::cerr << "acc list size: " << acc_list.size() << std::endl;;
			if (id == -1){

				if (megu_list.size() == 0){
					LoadMegu(megu_id, 1);
					megu_list[1].owner_name = owner_name;
					megu_list[1].pet_name = pet_name;
					
					megu_list[1].position.x = 200;
					megu_list[1].position.y = 200;
					if (mini){
						megu_list[1].position.x /= 2;
						megu_list[1].position.y /= 2;
					}
					megu_list[1].shadow_pos.x = megu_list[1].position.x + shadow_pos.x;
					megu_list[1].shadow_pos.y = megu_list[1].position.y + shadow_pos.y;
					megu_list[1].shadow_pos.w = shadow_pos.w;
					megu_list[1].shadow_pos.h = shadow_pos.h;
					megu_list[1].blink = Megu::OPEN;
					megu_list[1].state = Megu::STOP_ONE;
			/*		for (std::vector<Megu::scene_objects>::iterator it = acc_list.begin(); it != acc_list.end(); ++it){
						LoadAccessory(it->id, 1, it->offset.x, it->offset.y, 0, 0, it->type, it->offset_side);
					}*/
				}
				else {
					int p_id = megu_list.rbegin()->first + 1;
					LoadMegu(megu_id, p_id);
					megu_list[p_id].owner_name = owner_name;
					megu_list[p_id].pet_name = pet_name;
					megu_list[p_id].position.x = 200;
					megu_list[p_id].position.y = 200;
					if (mini){
						megu_list[p_id].position.x /= 2;
						megu_list[p_id].position.y /= 2;
					}
					megu_list[p_id].shadow_pos.x = megu_list[p_id].position.x + shadow_pos.x;
					megu_list[p_id].shadow_pos.y = megu_list[p_id].position.y + shadow_pos.y;
					megu_list[p_id].blink = Megu::OPEN;
					megu_list[p_id].state = Megu::STOP_ONE;
				/*	for (std::vector<Megu::scene_objects>::iterator it = acc_list.begin(); it != acc_list.end(); ++it){
						LoadAccessory(it->id, p_id, it->offset.x, it->offset.y, 0, 0, it->type, it->offset_side);
					}*/
				}
			}
			else{
				LoadMegu(megu_id, id);
				megu_list[id].owner_name = owner_name;
				megu_list[id].pet_name = pet_name;
				megu_list[id].position.x = 200;
				megu_list[id].position.y = 200;
				if (mini){
					megu_list[id].position.x /= 2;
					megu_list[id].position.y /= 2;
				}
				megu_list[id].shadow_pos.x = megu_list[id].position.x + shadow_pos.x;
				megu_list[id].shadow_pos.y = megu_list[id].position.y + shadow_pos.y;
				megu_list[id].blink = Megu::OPEN;
				megu_list[id].state = Megu::STOP_ONE;
			/*	for (std::vector<Megu::scene_objects>::iterator it = acc_list.begin(); it != acc_list.end(); ++it){
					LoadAccessory(it->id, id, it->offset.x, it->offset.y, 0, 0, it->type, it->offset_side);
				}*/
			}
		}
		else{
			std::cerr << "error opening file" << std::endl;
		}
		SDL_UnlockMutex(mutex);
	}
	
	SDL_DestroyMutex(mutex);
}

 void MeguEngine::serializeHouse(const std::string &filename){
	 std::ofstream file;
	 std::string path = "gamedata/house/SAV" + filename + ".house";
	 file.open(path);
	 if (file.is_open()){
		 //house first
		 file << "SAV" << filename << ".png\r\n";
		 file << house_id << "\r\n";
		 //then objects
		 int num_objs = objects_list.size();
		 file << num_objs << "\r\n";
		 for (int i = 0; i < num_objs; ++i){
			 file << objects_list[i].id << "\r\n";
			 file << objects_list[i].type << "\r\n";
			 file << objects_list[i].render_id << "\r\n";
			 if (mini == false){
				 file << objects_list[i].rectangle.x << "\r\n";
				 file << objects_list[i].rectangle.y << "\r\n";
			 }
			 else{
				 file << objects_list[i].rectangle.x*2 << "\r\n";
				 file << objects_list[i].rectangle.y*2 << "\r\n";
			 }
			 if (objects_list[i].side == SDL_FLIP_NONE){
				 file << 0 << "\r\n";
			 }
			 if (objects_list[i].side == SDL_FLIP_HORIZONTAL){
				 file << 1 << "\r\n";
			 }
		 }
		 file.close();
	 }
	 else{
		 std::cerr << "error saving file: " << path << std::endl;
	 }
 }
 void MeguEngine::deserializeHouse(const std::string &filename){
	 std::string path = "gamedata/house/SAV" + filename + ".house";
	 objects_list.clear();
	 std::ifstream file;
	 file.open(path.c_str());
	 if (file.is_open()){
		 std::string dump;
		 file >> dump;
		 //house first
		 file >> house_id;
		 updateBackground(house_id);
		 //then objects
		 int num_objs = 0;
		 file >> num_objs;
		 for (int i = 0; i < num_objs; ++i){
			 int id = 0;
			 int x, y;
			 int render_id;
			 std::string type = "";
			 SDL_RendererFlip side;
			 file >> id;
			 file >> type;
			 file >> render_id;
			 file >> x;
			 file >> y;
			 int sside;
			 file >> sside;
			 if (sside == 0){
				 side = SDL_FLIP_NONE;
			 }
			 else if (sside == 1){
				 side = SDL_FLIP_HORIZONTAL;
			 }

			 LoadFurniture(id,type,side);
			 if (mini == false){
				 objects_list.rbegin()->rectangle.x = x;
				 objects_list.rbegin()->rectangle.y = y;
			 }
			 else{
				 objects_list.rbegin()->rectangle.x = x/2;
				 objects_list.rbegin()->rectangle.y = y/2;
			 }
			 objects_list.rbegin()->render_id = render_id;
		 }
		 file.close();
	 }
	 else{
		 std::cerr << "error loading file: " << path << std::endl;
		 updateBackground(1);
		 serializeHouse("DEFAULT");//save new file
	 }
 }
 void MeguEngine::populateRndMsg(){
	 rapidxml::file<> *hxmlFile;
	 hxmlFile = new rapidxml::file<>("assets//xml//random_message.xml"); // Default template is char
	 rapidxml::xml_document<> hdoc;
	 hdoc.parse<0>(hxmlFile->data());
	 rapidxml::xml_node<> *hpRoot = hdoc.first_node();
	 int i = 0;
	 for (rapidxml::xml_node<> *bNode = hpRoot->first_node("message"); bNode; bNode = bNode->next_sibling("message"))
	 {
		 random_message_list[i] = bNode->value();
		 ++i;
	 }
	 delete hxmlFile;
	 hxmlFile = new rapidxml::file<>("assets//xml//pet_message.xml"); // Default template is char
	 hdoc.parse<0>(hxmlFile->data());
	 hpRoot = hdoc.first_node();
	 i = 0;
	 for (rapidxml::xml_node<> *bNode = hpRoot->first_node("message"); bNode; bNode = bNode->next_sibling("message"))
	 {
		 pet_message_list[i] = bNode->value();
		 ++i;
	 }

 }
 void MeguEngine::takeScreenshot(const std::string &path, const int &w, const int &h){
	 SDL_Surface* saveSurface = NULL;
	 SDL_Surface* copySurface = NULL;
	 SDL_Surface* infoSurface = NULL;

	 infoSurface = SDL_GetWindowSurface(win);
	 if (infoSurface == NULL) {
		 std::cerr << "Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
	 } else {
		 unsigned char * pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
		 if (pixels == 0) {
			 std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";
			 return ;
		 }
		 else {
			 if (SDL_RenderReadPixels(renderer, &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
				 std::cerr << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";
				 pixels = NULL;
				 return;
			 }
			 else {
				 copySurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w, infoSurface->w, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
				 if (copySurface == NULL) {
					 std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
					 return;
				 }
				 saveSurface = SDL_CreateRGBSurface(infoSurface->flags,
					 w,
					 h,
					 infoSurface->format->BitsPerPixel,
					 infoSurface->format->Rmask,
					 infoSurface->format->Gmask,
					 infoSurface->format->Bmask,
					 infoSurface->format->Amask);
				 if (saveSurface == NULL) {
					 std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
					 return;
				 }
				 SDL_BlitScaled(copySurface, NULL, saveSurface, NULL);

				 SDL_FreeSurface(copySurface);
				 std::string t_path = path + ".png";
				 std::cerr << t_path << std::endl;
				 SDL_SaveBMP(saveSurface, t_path.c_str());
				 SDL_FreeSurface(saveSurface);
				 saveSurface = NULL;
			 }
			 delete[] pixels;
		 }
		 
		 SDL_FreeSurface(infoSurface);
		 infoSurface = NULL;
	 }
 }
 void MeguEngine::loadFriendList(){
	 friends_save_list.clear();
#ifdef __APPLE__

	 DIR* dirFile = opendir("gamedata/friends");
	 if (dirFile)
	 {
		 struct dirent* hFile;
		 errno = 0;
		 while ((hFile = readdir(dirFile)) != NULL)
		 {
			 if (!strcmp(hFile->d_name, ".")) continue;
			 if (!strcmp(hFile->d_name, "..")) continue;

			 // in linux hidden files all start with '.'
			 if ((hFile->d_name[0] == '.')) continue;

			 // dirFile.name is the name of the file. Do whatever string comparison
			 // you want here. Something like:
			 if (strstr(hFile->d_name, ".megu")){
				 if (std::string(hFile->d_name) != "player.megu"){
					 std::string filename = std::string(hFile->d_name);
					 friends_save_list.push_back(filename);
				 }
				 else {
					 std::cout << "skipping" << std::endl;
				 }
			 }
			 closedir(dirFile);
		 }
	 }
#elif WIN32


	 DIR *dp;
	 struct dirent *dirp;
	 dp = opendir("gamedata/friends");
	 if (dp == NULL) {
		 return;
	 }
	 else{

		 while ((dirp = readdir(dp)) != NULL) {

			 if (!strcmp(dirp->d_name, ".")) continue;
			 if (!strcmp(dirp->d_name, "..")) continue;
			 std::string fn = dirp->d_name;
			 if (fn.substr(fn.find_last_of(".") + 1) == "megu") {
				 if (std::string(dirp->d_name) != "player.megu"){
					// std::string filename = "gamedata/friends/" + fn;
					 std::cerr << fn << std::endl;
					 friends_save_list.push_back(fn);
				 }
			 }
			 else {
				 std::cout << "skipping" << std::endl;
			 }
		 }
     }
#endif
 }

 void MeguEngine::serializeOptions(){
	 std::ofstream file;
	 file.open("gamedata//settings.dat");
	 if (file.is_open()){
		 //house first
		 file << options_menu->getBGMLevel() << "\r\n";
		 file << options_menu->getSFXLevel() << "\r\n";
		 file << options_menu->getMSGOn();
		 file.close();
	 }
	 else{
		 std::cerr << "error saving file: " << "gamedata//settings.dat" << std::endl;
	 }
 }

 void  MeguEngine::deserializeOptions(){
	 std::ifstream file;
	 int bgmlevel = 5;
	 int sfxlevel = 5;
	 bool msgon = true;
	 file.open("gamedata//settings.dat");
	 if (file.is_open()){
		 //house first
		 file >> bgmlevel;
		 file >> sfxlevel;
		 file >> msgon;
		 file.close();
	 }
	 else{
		 std::cerr << "loading files at, setting to default: " << "gamedata//settings.dat" << std::endl;
	 }
	 options_menu->setBGMLevel(bgmlevel);
	 options_menu->setMSGOn(msgon);
	 options_menu->setSFXLevel(sfxlevel);
	 sounds.setBGMVolume(bgmlevel);
	 sounds.setSFXVolume(sfxlevel);
	 message_screen->locked = msgon;
 }

 void MeguEngine::loadLayouts(){
	 layout_save_list.clear();
#ifdef __APPLE__

	 DIR* dirFile = opendir("gamedata/house");
	 if (dirFile)
	 {
		 struct dirent* hFile;
		 errno = 0;
		 while ((hFile = readdir(dirFile)) != NULL)
		 {
			 if (!strcmp(hFile->d_name, ".")) continue;
			 if (!strcmp(hFile->d_name, "..")) continue;

			 // in linux hidden files all start with '.'
			 if ((hFile->d_name[0] == '.')) continue;

			 // dirFile.name is the name of the file. Do whatever string comparison
			 // you want here. Something like:
			 if (strstr(hFile->d_name, "house")){
				 if (std::string(hFile->d_name) != "SAVDEFAULT"){
					 std::string filename = std::string(hFile->d_name);
					 std::cerr << "layout: "<< filename << std::endl;
					 layout_save_list.push_back(filename);
				 }
				 else {
					 std::cout << "skipping" << std::endl;
				 }
			 }
			 closedir(dirFile);
		 }
	 }
#elif WIN32


	 DIR *dp;
	 struct dirent *dirp;
	 dp = opendir("gamedata/house");
	 if (dp == NULL) {
		 return;
	 }
	 else{

		 while ((dirp = readdir(dp)) != NULL) {

			 if (!strcmp(dirp->d_name, ".")) continue;
			 if (!strcmp(dirp->d_name, "..")) continue;
			 std::string fn = dirp->d_name;
			 if (fn.substr(fn.find_last_of(".") + 1) == "house") {
				 if (std::string(dirp->d_name) != "SAVDEFAULT.house"){
					 std::cerr << "layout: " << fn << std::endl;
					 layout_save_list.push_back(fn);
					 
				 }
			 }
			 else {
				 std::cout << "skipping" << std::endl;
			 }
		 }
	 }
#endif

 }