#pragma once

#include <iostream>
#include "../Tier.h"
#include <map>
#include <algorithm> 
#include <string>
#include "../Helpers/PathHelper.h"
#include "../MetadataCache.h"
#include "../Models/Role.h"

using namespace std;



	class Roles
	{
		public:
			map<string, Role> list =
			{
				{"noob", Role("Noob", "NOOB  ", "!noob")},
				{"guest", Role("Guest", "> ", "!guest")},
				{"mod", Role("Mod", "MOD  ", "!nonconflictingmod")},
				{"host", Role("Host")},
				{"z", Role("+Add Role", "ROLE ERROR?", "", "z")}
			};

			static Roles r;
			bool SaveToFile();
			map<string, Role> LoadFromFile();
	};
	

	
	class GuestRoles
	{
	public:

	public:
		void setRole(uint32_t userID, Role role);
		Role getRole(uint32_t userID);

		bool saveRoles();
		void loadRoles();

		static GuestRoles instance;
	private:
		vector<GuestRole> _guestRoles;
	};

