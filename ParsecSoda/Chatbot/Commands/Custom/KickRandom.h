#pragma once

#include "../../ACommand.h"
#include "parsec-dso.h"
#include "../../../Helpers/Dice.h"


class KickRandom : public ACommand
{
public:

	std::string usage = "Usage: /randkick";

	/**
	 * @brief Construct a new CommandKick object
	 *
	 * @param msg
	 * @param sender
	 * @param parsec
	 * @param guests
	 * @param isHost
	 */
	KickRandom(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, bool isHost = false)
		: ACommand(msg, sender), _parsec(parsec), _isHost(isHost), guests(guests), sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		// Find the guest
		if (!findGuest()) {
			setReply("Can't find a random guest!\0");
			return false;
		}

		Tier tier = Cache::cache.tierList.getTier(target.userID);
		if (tier >= Tier::ADMIN)
		{
			setReply(target.name + " has immunity to being kicked!");
			return true;
		}

		// Kicked themselves...?
		if (sender.userID == target.userID) {
			setReply(sender.name + " kicked themselves in confusion!");
			kickGuest();
			return true;
		}

		// Can't kick a Soda Cop
		if (Cache::cache.isSodaCop(target.userID)) {
			setReply("Nice try bub, but you can't kick a Soda Cop!\0");
			return false;
		}

		// Kick the guest
		setReply(
			Cache::cache.isSodaCop(sender.userID) ?
			sender.name + " laid down the law as a Soda Cop and kicked " + target.name + "!\0"
			: sender.name + " kicked " + target.name + "!\0"
		);
		kickGuest();

		try {
			PlaySound(TEXT("./SFX/kick.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
		}
		catch (const std::exception&) {}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/randkick" };
	}

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/randkick " };
	}

	void kickGuest() {
		if (target.fake) {
			for (int i = 0; i < guests.getGuests().size(); ++i) {
				if (guests.getGuests()[i].userID == target.userID) {
					guests.getGuests().erase(guests.getGuests().begin() + i);
					break;
				}
			}
			return;
		}
		ParsecHostKickGuest(_parsec, target.id);
	}

	/**
	* Get the guest referenced in the command. Returns nullptr
	* if no guest is found
	*
	* @param guestList The guest list
	*/
	bool findGuest() {

		// Get the guest
		try {
			int num = Dice::number(guests.getGuests().size());
			target = guests.getGuests()[num];
			return true;
		}
		catch (const std::exception&) {
			setReply("Evil error message that you're not supposed to see!\0");
			return true;		
		}
		return false;
	}

	Guest target;
	Guest& sender;
	GuestList guests;
	ParsecDSO* _parsec;
	bool _isHost;
};