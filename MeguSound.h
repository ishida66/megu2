#pragma once
#include "SDL_include.h"
#include <map>
#include <string>

typedef std::string sfx_name;

class MeguSound{
private:

	std::map<sfx_name, Mix_Music*> bgm;
	std::map<sfx_name, Mix_Chunk*> sfx;
	bool bgm_on;
	bool sfx_on;
	
public:

	void playSFX(const sfx_name &name, const int &channel);
	void playBGM(const sfx_name &name);
	void setSFX(const sfx_name &name, const char *filename);
	void setBGM(const sfx_name &name, const char *filename);
	void cutSFX(const int &channel);
	void pauseMusic();
	void resumeMusic();
	void loadAll();
	void setBGMOn(const bool &bgm);
	void setSoundOn(const bool &bgm);
	bool getBGMOn();
	bool getSoundOn();
	void setBGMVolume(const int &bgm);
	void setSFXVolume(const int &sfx);
	~MeguSound();
	int sfx_volume;
	int bgm_volume;
};