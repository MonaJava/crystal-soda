#pragma once

#include <iostream>
#include <algorithm> 
#include <string>
#include "../Helpers/PathHelper.h"


using namespace std;
class Role
{
public:
	Role()
	{
		this->name = "Guest";
		this->messageStarter = "";
		this->commandPrefix = "!guest";
		this->key = "pleb";
	}
	Role(std::string n)
	{
		this->name = n;
		this->messageStarter = n + "  ";
		string lowercase = n;
		transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
		this->commandPrefix = "!" + lowercase;
		this->key = lowercase;
	}
	Role(std::string n, std::string m, std::string c)
	{
		this->name = n;
		this->messageStarter = m;
		this->commandPrefix = c;
		string lowercase = n;
		transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
		this->key = lowercase;
	}
	Role(std::string n, std::string m, std::string c, std::string k)
	{
		this->name = n;
		this->messageStarter = m;
		this->commandPrefix = c;
		this->key = k;
	}

	std::string name;
	std::string messageStarter;
	std::string commandPrefix;
	std::string key;
	int extraHotseatTime;
	int cooldownShrink;
	int rank;
};


class GuestRole
{
public:
	GuestRole(uint32_t userID, Role role) {
		this->userID = userID;
		this->role = role;
	}
	uint32_t userID;
	Role role;
};