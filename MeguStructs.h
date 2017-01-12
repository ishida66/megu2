
#include "SDL_include.h"
#include <vector>
#include <map>
#include <random>
namespace Game{
	enum GAME_STATE{
		ERROR,
		INIT,
		STOP,
		RUNNING,
		EDIT_MODE,
		ACCESSORY_MODE,
		FOOD_MENU,
		FURNITURE_MENU,
		PET_MENU,
		HOUSE_MENU,
		ACCESSORY_MENU,
		CARE_MENU,
		EDIT_MENU,
		STATUS_MENU,
		NAME_MENU,
		SAVE_MEGU_MENU,
		SAVE_HOUSE_MENU,
		LOAD_MEGU_MENU,
		LOAD_HOUSE_MENU,
		NEW_MEGU_NAME_MENU,
		NEW_MEGU_PET_MENU,
		FRIEND_INVITE_MENU,
		OPTIONS_MENU,
		LAYOUT_SAVE_MENU,
		LAYOUT_LOAD_MENU,
		FEEDING
		
		
		

	};
}
namespace Megu{
	typedef int ID;
	typedef int MEGU_SIZE;

	enum MEGU_STATE{
		WALK_ONE = 0,
		WALK_TWO = 1,
		STOP_ONE = 2,
		STOP_TWO = 3,
		SCARED = 6,
		VERY_HAPPY = 7,
		HAPPY = 8,
		AFRAID = 9,
		THINKING = 10
	};

	enum MEGU_EYE{
		NONE = -1,
		OPEN = 4,
		BLINK = 5,
		BLINKED = 6
	};
	//for food positioning
	struct food_kit{
		bool visible;
		//table
		SDL_Texture *table;
		//table shadow
		SDL_Texture *table_shadow;
		//table pos
		SDL_Rect rect_t;
		//shadow pos
		SDL_Rect rect_s;
		//even megu id position to go
		SDL_Rect even_megu;
		//odd megu id position to go
		SDL_Rect odd_megu;
		//food texture
		std::vector<SDL_Texture *> food;
		//food position
		SDL_Rect rect_f;
		void destroy_food_kit(){
			food.clear();
			visible = false;
		}
	};

	struct scene_objects{
		ID id; //unique ID
		ID render_id; //unique ID each time an object is created
		SDL_Rect rectangle;//position
		SDL_Rect offset;//position
		SDL_RendererFlip offset_side; //offset side
		std::string type; //for foods as furniture specifically
		int animation_interval;//if required
		int current_frame;
		int current_time;
		std::vector<int> animation_order;
		std::vector<SDL_Texture *> textures; //if required
		SDL_RendererFlip side; // SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL
		bool IsIn(int mouseX, int mouseY)
		{
			if (((mouseX > rectangle.x) && (mouseX < rectangle.x + rectangle.w))
				&& ((mouseY > rectangle.y) && (mouseY < rectangle.y + rectangle.h))
				) {
				return true;
			}
			else {
				return false;
			}
		}
	};

	struct megu_sprites{
		int id; //megu id
		std::string pet_name; //name of your megu
		std::string owner_name; //name of owner
		std::string pet_race; //race of megu
		int pet_counting; //number of pets done in sequence, after 8 gets super happy :)
		uint32_t pet_timer; //hover mouse over megu over a certain threshold trigger pet
		std::default_random_engine generator;
		SDL_Texture* sprites[11];
		SDL_Texture* shadow;
		SDL_Rect shadow_pos;
		Megu::MEGU_SIZE size;
		SDL_Rect position; //current position
		SDL_Rect destination; // destination position
		bool run_flag; //if running 1
		bool pet_flag; //if petting
		std::vector<SDL_Texture *> heart_textures;
		std::vector<scene_objects> accessories_list;//acessories
		int accessory_index;//selected accessories
		MEGU_STATE state;
		MEGU_EYE blink; //if open 4, blink 5
		uint32_t blink_timer; //eye blinking trigger
		uint32_t blink_openeye; //open eye timer
		uint32_t animation_timer; //animation switching
		uint32_t misc_timer; //happy, sad etc
		uint32_t random_action_timer; //walking and sounds
		uint32_t heart_animation_timer;// heart animation timer
		enum heart_animation_state{
			STOP = 0,
			FIRST = 1,
			SECOND = 2,
			THIRD = 3			
		}heart_animation;
		SDL_RendererFlip side; // SDL_FLIP_NONE or SDL_FLIP_HORIZONTAL

		~megu_sprites(){
			for (int i = 0; i < 11; ++i){
				SDL_DestroyTexture(sprites[i]);
				SDL_DestroyTexture(shadow);
			}
		}
		megu_sprites(){
			blink_timer = 0;
			animation_timer = 0;
			pet_timer = 0;
			run_flag = false;
			misc_timer = 0;
			pet_counting = 0;
			pet_flag = 0;
			random_action_timer = 0;
			heart_animation_timer = 0;
			heart_animation = STOP;
		}
	};
};


typedef std::string MEGU_NAME;
typedef unsigned int PLAYER_ID;

struct render_ordering{
	PLAYER_ID player;
	int position_y;
	render_ordering(){
		player = -1;
		position_y = 0;
	}
	render_ordering(PLAYER_ID p, int _y){
		player = p;
		position_y = _y;

	}
};
