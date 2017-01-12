#pragma once
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include <SDL2/SDL.h>
#include"MeguStructs.h"

class MeguSerialize{
	friend class MeguEngine;
public:
	static void serializePlayer(const std::string &path, Megu::megu_sprites &player){
		std::ofstream file;
		file.open(path.c_str());
		if (file.is_open()){
			file << player.owner_name << "/n";
			file << player.pet_name << "/n";
			file << player.id << "/n";
			file.close();
		}
		else{
			std::cerr << "error opening file" << std::endl;
		}
	}
	static void deserializePlayer(const std::string &path, const int id, SDL_Renderer *renderer){
		std::ifstream file;
		std::string line;

		file.open(path.c_str());
		int megu_id;
		std::string owner_name;
		std::string pet_name;
		if (file.is_open()){
			file >> owner_name;
			file >> pet_name;
			file >> megu_id;
			file.close();
		}
		else{
			std::cerr << "error opening file" << std::endl;
		}
	}
	static void serializeHouse(const std::string &path, std::vector<Megu::scene_objects> &objects_list);
	static void deserializeHouse(const std::string &path, std::vector<Megu::scene_objects> &objects_list, SDL_Renderer *renderer);
};