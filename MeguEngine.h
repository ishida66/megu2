#pragma once
#include "MeguSound.h"
#include "MeguStructs.h"
#include"items_library.h"
#include <string>
#include <map>
#include <vector>
#include <list>
#include "SDL_include.h"
#include "MeguButton.h"
#include "AccessoryMenus.h"
#include "FoodMenus.h"
#include "FurnitureMenus.h"
#include "CareMenus.h"
#include "PetMenus.h"
#include "HouseMenus.h"
#include "CareMenus.h"
#include "EditMenus.h"
#include "EditMenus.h"
#include "NameMenus.h"
#include "FriendsMenus.h"
#include "LayoutMenus.h"
#include "OptionsMenus.h"
#include "SizeMenus.h"
#include "MessageScreen.h"
#include "TtfWrapper.h"
#include <random>


class MeguEngine {
private:
	bool mini;
	SDL_mutex *mutex;
	unsigned int player_counter;
	//food kit
	Megu::food_kit kit;
	//game state
	Game::GAME_STATE status;
	//menu variables
	bool menu_open;//disable stuff if any menu is opened
	MeguSound sounds;
	//timer thresholds
	const uint32_t megu_blink_counter;
	const uint32_t megu_openeye_counter;
	const uint32_t megu_animation_counter;
	const uint32_t megu_misc_counter;
	const uint32_t megu_pet_counter;
	const uint32_t megu_event_action;
	const uint32_t megu_heart_event_counter;
	const uint32_t megu_msg_counter;
	uint32_t curr_time;
	uint32_t last_time;
	uint32_t msg_time;
	//megu speed
	const int megu_spd;
	const uint32_t scene_limit;
	//ID mapping
	bool Running;
	//players list
	std::vector<PLAYER_ID> player_list;
	//megu player map association
	std::map<PLAYER_ID, Megu::megu_sprites> megu_list;
	//object in scene list
	std::vector<Megu::scene_objects> objects_list;
	std::map<int, std::string> random_message_list;
	std::map<int, std::string> pet_message_list;
	//current house id
	int house_id;

	bool edit_holding;
	int object_index;
	//SDL render variables
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *menu_bar = NULL; // menu bar texture
	SDL_Texture *house = NULL; //house texture
	SDL_Rect clickable_area;
	SDL_Texture *mouse_cursor = NULL;
	//SDL_Rect Positioning
	SDL_Rect menu_bar_pos;
	SDL_Rect menu_food_pos;
	SDL_Rect menu_furniture_pos;
	SDL_Rect menu_background_pos;
	item_library library;
	SDL_Point mPosition;
	SDL_Rect shadow_pos;
	std::string shadow_texture;
	SDL_Rect megu_size;
	int shadow_alpha;
	//button widgets
	widgets::MeguButton *care_button;
	widgets::MeguButton *edit_button;
	widgets::MeguButton *option_button;
	widgets::MeguButton *return_button;
	//menu
	widgets::FoodMenus *food_menu;
	widgets::FurnitureMenus *furniture_menu;
	widgets::PetMenus *pet_menu;
	widgets::HouseMenus *house_menu;
	widgets::CareMenus *care_menu;
	widgets::EditMenus *edit_menu;
	widgets::NameMenus *name_menu;
	widgets::OptionsMenus *options_menu;
	widgets::FriendsMenus *friends_menu;
	widgets::LayoutMenus *layout_menu;
	widgets::SizeMenus *size_menu;
	widgets::AccessoryMenus *accessory_menu;
	widgets::MessageScreen *message_screen;
	std::vector<render_ordering> render_order;
	std::vector<std::string> friends_save_list;
	std::vector<std::string> layout_save_list;
	std::vector<SDL_Texture*> hearts_texture;
	SDL_Rect heart_offset;
	ID id_accessory;
	
	void populateLib(widgets::MeguMenus &menu, std::map<ID, item_library::item> &item_lib);
	void populateHouseLib(widgets::MeguMenus &menu, std::map<ID, item_library::house> &item_lib);
	void populatePetLib(widgets::MeguMenus &menu, std::map<ID, item_library::megu> &pet_lib);
	void populateRndMsg();
	inline void getConstrainedMovement(const int &wx, const int &wy, int &out_wx, int &out_wy);
public:

	MeguEngine();

	int OnExecute();
	
	//basic functions
	//Loads individual image as texture
	SDL_Texture* loadTexture(std::string path);

	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//Current displayed texture
	SDL_Texture* gTexture = NULL;

	//init method
	bool OnInit();

	//init menus
	void initMenus();
	//init objects
	void initObjects();
	//event triggered process
	void OnEvent(SDL_Event* Event);

	//loop process
	void OnLoop();

	//rendering process
	void OnRender();

	void OnCleanup();

	//specific functions
	//load house background
	bool updateBackground(const int &id);

	//megu name, player
	void LoadMegu(const int &megu_id, const PLAYER_ID &player);

	void LoadFurniture(const int &furniture_id, const std::string &type, const SDL_RendererFlip &side = SDL_FLIP_NONE);

	void LoadAccessory(const int &accessory_id, const int &player_id, const int &off_x, const int &off_y, const int &pos_x, const int &pos_y, const std::string &type, const SDL_RendererFlip &side = SDL_FLIP_NONE);
	
	void SetMeguPos(const PLAYER_ID &player, const int &x, const int &y, const SDL_RendererFlip &side = SDL_FLIP_NONE);

	//set Megu Position
	void SetMeguState(const PLAYER_ID &player, const Megu::MEGU_STATE &new_state, const Megu::MEGU_EYE &eye_state = Megu::NONE);

	//Megu State Machine
	void MeguStateMachine();

	//Megus Position Handler, manages movement across the map
	void MeguPositionHandler(const PLAYER_ID&player);
	//Set Megu Destination for movement.
	void SetMeguDestination(const PLAYER_ID &player, const int &x, const int &y);

	//Click on scene callback function
	void MeguClickScene(const PLAYER_ID &player, const int &x, const int&y);
	//MouseEvents	
	//Sets top left position
	void setMousePosition(int x, int y);
	//Handles mouse event
	void handleMouseEvent(SDL_Event* e);
	//Shows button sprite
	void mouserender();
	//sort render order
	void SetRenderOrder();
	//render food table
	void food_mode(const int &ID);

	void saveGame();

	void importMegu();

	void foodAnimationCallback(const PLAYER_ID &player);

	void serializePlayer(const std::string &path, Megu::megu_sprites &player);
	
	void deserializePlayer(const std::string &path, const int &id = -1);
	
	void serializeHouse(const std::string &filename);

	void deserializeHouse(const std::string &filename);

	void loadFriendList();

	void loadLayouts();

	void takeScreenshot(const std::string &path, const int &w, const int &h);

	void loadMessages();

	void serializeOptions();

	void deserializeOptions();

	void handleSizeSelector(SDL_Event *e);
protected:
	std::vector<TtfWrapper> ui_texts;
	int new_megu_id;
};

