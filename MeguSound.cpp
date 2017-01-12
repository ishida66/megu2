#include "MeguSound.h"
#include "SDL_include.h"


void MeguSound::playSFX(const sfx_name &name, const int &channel){
	if (sfx_on){
		Mix_PlayChannel(channel, sfx[name], 0);
		Mix_Volume(channel, sfx_volume*24);
	}
}
void MeguSound::playBGM(const sfx_name &name){
	if (bgm_on){
		if (Mix_PlayMusic(bgm[name], -1) == -1){
			printf("Mix_PlayMusic: %s\n", Mix_GetError());
		}
		Mix_VolumeMusic(bgm_volume*6);
	}
}

void MeguSound::setSFX(const sfx_name &name, const char *filename){
	sfx[name] = Mix_LoadWAV(filename);
}
void MeguSound::setBGM(const sfx_name &name, const char* filename){
	bgm[name] = Mix_LoadMUS(filename);
}

void MeguSound::setBGMVolume(const int& volume){
	bgm_volume = volume;
	Mix_VolumeMusic(volume*6);
}
void MeguSound::loadAll(){
	setBGM("stroll", "assets/sound/BGM_stroll.mp3");
	setBGM("race", "assets/sound/BGM_race.mp3");
	setBGM("house", "assets/sound/MEGU_BGM.mp3");
	setBGM("adventure", "assets/sound/BGM_Adventure.mp3");
	setSFX("bad", "assets/sound/sfx/SE_bad.wav");
	setSFX("balltoss", "assets/sound/sfx/SE_balltoss.wav");
	setSFX("beat", "assets/sound/sfx/SE_beat.wav");
	setSFX("changed", "assets/sound/sfx/SE_changed.wav");
	setSFX("changing", "assets/sound/sfx/SE_changing.wav");
	setSFX("click", "assets/sound/sfx/SE_click.wav");
	setSFX("Complete", "assets/sound/sfx/SE_Complete.wav");
	setSFX("Continuation", "assets/sound/sfx/SE_Continuation.wav");
	setSFX("Departure", "assets/sound/sfx/SE_Departure.wav");
	setSFX("elixir", "assets/sound/sfx/SE_elixir.wav");
	setSFX("food", "assets/sound/sfx/SE_food.wav");
	setSFX("GetCntinuation", "assets/sound/sfx/SE_GetCntinuation.wav");
	setSFX("gold", "assets/sound/sfx/SE_gold.wav");
	setSFX("GotTheItem", "assets/sound/sfx/SE_beat.wav");
	setSFX("happy1", "assets/sound/sfx/SE_happy1.wav");
	setSFX("happy2", "assets/sound/sfx/SE_happy2.wav");
	setSFX("happy3", "assets/sound/sfx/SE_happy3.wav");
	setSFX("levelup", "assets/sound/sfx/SE_levelup.wav");
	setSFX("megu1", "assets/sound/sfx/SE_megu1.wav");
	setSFX("megu2", "assets/sound/sfx/SE_megu2.wav");
	setSFX("megu3", "assets/sound/sfx/SE_megu3.wav");
	setSFX("megu4", "assets/sound/sfx/SE_megu4.wav");
	setSFX("megu5", "assets/sound/sfx/SE_megu5.wav");
	setSFX("megu6", "assets/sound/sfx/SE_megu6.wav");
	setSFX("megu7", "assets/sound/sfx/SE_megu7.wav");
	setSFX("megu8", "assets/sound/sfx/SE_megu8.wav");
	setSFX("megu9", "assets/sound/sfx/SE_megu10.wav");
	setSFX("megu10", "assets/sound/sfx/SE_megu10.wav");
	setSFX("notsobad", "assets/sound/sfx/SE_notsobad.wav");
	setSFX("pet", "assets/sound/sfx/SE_pet.wav");
	setSFX("Stamp", "assets/sound/sfx/SE_Stamp.wav");
	setSFX("sweep", "assets/sound/sfx/SE_sweep.wav");
	setSFX("wash", "assets/sound/sfx/SE_wash.wav");
	bgm_on = true;
	sfx_on = true;
	sfx_volume = 120;
	bgm_volume = 30;
}

void MeguSound::cutSFX(const int &channel){
	Mix_Pause(channel);
}

void MeguSound::pauseMusic(){
	Mix_PauseMusic();
}
void MeguSound::resumeMusic(){
	Mix_ResumeMusic();
}
MeguSound::~MeguSound(){
	bgm.clear();
	sfx.clear();
}

void MeguSound::setBGMOn(const bool &bgm){
	bgm_on = bgm;
}
void MeguSound::setSoundOn(const bool &sfx){
	sfx_on = sfx;
}
bool MeguSound::getBGMOn(){
	return bgm_on;
}
bool MeguSound::getSoundOn(){
	return sfx_on;
}

void MeguSound::setSFXVolume(const int &sfx){
	sfx_volume = sfx;
}