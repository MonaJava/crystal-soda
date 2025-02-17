#pragma once

#include "../../ACommand.h"
#include "../../../GamepadClient.h"

class CommandLimit : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandLimit object
	 * 
	 * @param msg
	 * @param guests
	 * @param gamepadClient
	 */
	CommandLimit(const char* msg, Guest& sender, GuestList &guests, GamepadClient &gamepadClient)
		: ACommand(msg, sender), _gamepadClient(gamepadClient), guests(guests)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		/*if (!ACommandSearchUserIntArg::run()) {
			SetReply("Usage: !limit <username> <number>\nExample: !limit melon 2");
			return false;
		}*/

		if (getArgs().size() == 0) {
			setReply("Usage: !limit <username> <number>\nExample: !limit melon 2");
			return false;
		}
		if (findGuest())
		{
			int _intArg = std::stoi(getArgs()[1]);

			_gamepadClient.setLimit(target.userID, _intArg);
			setReply("Gamepad limit set to " + to_string(_intArg) + " for " + target.name);
		}
		else
		{
			setReply("Guest not found");
			return false;
		}
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!limit" };
	}

protected:

	/**
	 * @brief Get the internal prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!limit " };
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

	GamepadClient& _gamepadClient;
	GuestList guests;
	Guest target;
};
