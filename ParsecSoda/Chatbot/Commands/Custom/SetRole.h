#pragma once

#include "../../../Core/Cache.h"
#include "../../ACommand.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include "parsec-dso.h"
#include "../Lists/Roles.h"

using namespace std;

class SetRole : public ACommand
{
public:

	/**
	 * @brief Construct a new SetRole object
	 *
	 * @param msg The message to parse
	 * @param sender The sender of the message
	 * @param parsec The parsec instance
	 * @param guests The list of online guests
	 * @param guestHistory The list of offline guests
	 */
	SetRole(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, GuestDataList& guestHistory, Role &role)
		: ACommand(msg, sender), _sender(sender), _parsec(parsec), _role(role), guests(guests)
	{
		
	}
	//vector<const char*> {strcat(role.commandPrefix, role.name)}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		//ACommandSearchUser::run();
		if (getArgs().size() == 0) {
			setReply("Usage: " + _role.commandPrefix +" <username>\0");
			return false;
		}
		if (findGuest()) {
			GuestData targetData(target.name, target.userID);
			return handleGuest(targetData);
		}
		else
		{
			setReply(_sender.name + ", I cannot find the user you want to make a " + _role.name + "\0");
		}
		//_role.commandPrefix.pop_back();
		bool rv = false;

		/*switch (_searchResult)
		{
		case SEARCH_USER_RESULT::NOT_FOUND:
			SetReply(_sender.name + ", I cannot find the user you want to make a " + _role.name + "\0");
			break;

		case SEARCH_USER_RESULT::FOUND:
			handleGuest(GuestData(_targetGuest.name, _targetGuest.userID), true, _targetGuest.id);
			break;


		case SEARCH_USER_RESULT::FAILED:
		default:
			SetReply("Usage: " + _role.commandPrefix + " <username>\nExample: " + _role.commandPrefix + " Call_Me_Troy\0");
			break;
		}*/

		return false;
	}


private:

	ParsecDSO* _parsec;
	Guest& _sender;
	Role& _role;
	Guest target;
	GuestList guests;

	/**
	 * @brief Handle the guest
	 *
	 * @param target The guest to handle
	 * @param isOnline True if the guest is online
	 * @param guestID The guest ID
	 * @return true if the guest was handled
	 */
	bool handleGuest(GuestData target) {
		bool result = false;
		GuestRoles::instance.setRole(target.userID, _role);

		setReply(target.name + " is now a " + _role.name + "!\0");
		return result;
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
};