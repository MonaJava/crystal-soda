#pragma once

#include "../../ACommand.h"
#include "parsec.h"
#include "../../../Modules/Hotseat.h"
#include "../../../Core/Config.h"

class CommandPlayTime : public ACommand
{
public:
	std::string usage = "Usage: !playtime <username>";

	/**
	 * @brief Construct a new CommandPlayTime object
	 * 
	 * @param msg 
	 * @param sender 
	 * @param guests 
	 * @param host 
	 */
	CommandPlayTime(const char* msg, Guest& sender, GuestList& guests)
		: ACommand(msg, sender), guests(guests)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		// Look for user
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		if (!findGuest()) {
			setReply("Can't find the guest you want to playtime!\0");
			return false;
		}
		else {
			setReply(target.name + " has " + Hotseat::instance.getUserTimeRemaining(target.userID)
				+ " of play time remaining. \nTheir play time will refresh in "
				+ Hotseat::instance.getCooldownRemaining(target.userID) + ".\0");
			return true;
		}



		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!playtime" };
	}

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!playtime " };
	}
	
	bool findGuest() {

		// Get the guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}

		try {
			uint32_t id = stoul(guest);
			vector<Guest>::iterator i;
			for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
				if ((*i).userID == id) {
					target = *i;
					return true;
				}
			}
		}
		catch (const std::exception&) {
			bool found = guests.find(guest, &target);
			if (found) {
				return true;
			}
		}

		return false;
	}

	Guest target;
	
	GuestList guests;
};