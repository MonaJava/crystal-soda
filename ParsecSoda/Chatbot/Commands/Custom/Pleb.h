#pragma once

#include "../../../Core/Cache.h"
#include "../../ACommand.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include "parsec-dso.h"
#include "../../../GamepadClient.h"

using namespace std;

class Pleb : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandPleb object
	 * 
	 * @param msg The message to parse
	 * @param sender The sender of the message
	 * @param parsec The parsec instance
	 * @param guests The list of online guests
	 * @param guestHistory The list of offline guests
	 */
	Pleb(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, GuestDataList& guestHistory, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _sender(sender), _parsec(parsec), _gamepadClient(gamepadClient), guests(guests), guesthistory(guestHistory)
	{
	}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		/*ACommandSearchUserHistory::run();

		bool rv = false;

		switch (_searchResult)
		{
		case SEARCH_USER_HISTORY_RESULT::NOT_FOUND:
			SetReply(_sender.name + ", I cannot find the user you want to make a non-noob.\0");
			break;

		case SEARCH_USER_HISTORY_RESULT::ONLINE:
			handleGuest(GuestData(_onlineGuest.name, _onlineGuest.userID), true, _onlineGuest.id);
			_guestHistory.pop(_onlineGuest.userID);
			break;

		case SEARCH_USER_HISTORY_RESULT::OFFLINE:
			handleGuest(_offlineGuest, false);
			_guestHistory.pop(_offlineGuest.userID);
			break;

		case SEARCH_USER_HISTORY_RESULT::FAILED:
		default:
			SetReply("Usage: /unnoob <username>\nExample: /unnoob Call_Me_Troy\0");
			break;
		}
		*/
		return true;
	}

	/**
	 * @brief Get the prefixes
	 * @return The prefixes
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/unnoob" };
	}

private:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "/unnoob " };
	}

	ParsecDSO* _parsec;
	Guest& _sender;
	GamepadClient& _gamepadClient;
	GuestList guests;
	GuestDataList guesthistory;

	/**
	 * @brief Handle the guest
	 * 
	 * @param target The guest to handle
	 * @param isOnline True if the guest is online
	 * @param guestID The guest ID
	 * @return true if the guest was handled
	 */
	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1) {
		bool result = false;

		if (_sender.userID == target.userID) {
			setReply("Skibidi sigma rizz, " + _sender.name + "!\0");
		} else {
			setReply(_sender.name + " has made " + target.name + " a non-noob!\0");
			_gamepadClient.setLimit(target.userID, 1);

			if (Cache::cache.noobExemptList.Add(target)) {
				
				Cache::cache.tierList.setTier(target.userID, Tier::PLEB);


				result = true;
			}
		}

		return result;
	}
};