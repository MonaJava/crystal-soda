#pragma once

#include "../../ACommand.h"
#include "parsec-dso.h"
#include "VoteCommands.h"


class VoteKick : public ACommand
{
public:

	std::string usage = "Usage: /votekick <username>";

	/**
	 * @brief Construct a new CommandKick object
	 *
	 * @param msg
	 * @param sender
	 * @param parsec
	 * @param guests
	 * @param isHost
	 */
	VoteKick(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, bool isHost = false)
		: ACommand(msg, sender), _parsec(parsec), _isHost(isHost), guests(guests), sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Was a guest specified?
		/*if (getArgs().size() == 0 && VoteData::instance._isVoting == false) {
			setReply(usage);
			return false;
		}*/

		// Find the guest
		if (!findGuest(getArgString()) && VoteData::instance._isVoting == false) {
			setReply("Can't find the guest you want to votekick!\0");
			return false;
		}

		Tier tier = Cache::cache.tierList.getTier(target.userID);
		if (tier >= Tier::GOD)
		{
			setReply(target.name + " is the host! If you don't like it, leave!");
			return true;
		}

		// Kicked themselves...?
		if (sender.userID == target.userID) {
			setReply(sender.name + " attempted to self-pity!");
			kickGuest();
			return true;
		}

		//Polling begin
		if (VoteData::instance._isVoting == false)
		{
			VoteKick* instance = new VoteKick(getMsg(), _sender, _parsec, guests, _isHost);
			VoteData::instance.setCommand(instance);
			setReply("Votekick initiated for " + target.name + "!\nType /yay to approve the kick or /nay to deny it!");
			return true;
		}

		int voteDifference = VoteData::instance._yayVotes - VoteData::instance._nayVotes;
		int checkNum = Config::cfg.room.guestLimit / 2;
		//Polling end with nay majority
		if (voteDifference < -checkNum)
		{
			setReply("Votekick denied. Perhaps there are some over-reactors among us?");
			return false;
		}

		//Polling end with yay majority
		if (voteDifference > checkNum)
		{
			setReply("Votekick approved! Let's get " + VoteData::instance._subject + " out of here.");
			findGuest(VoteData::instance._subject);
			kickGuest();
			try {
				PlaySound(TEXT("./SFX/kick.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
			}
			catch (const std::exception&) {}
			return true;
		}

		if (VoteData::instance._isVoting == true)
		{
			setReply("Something else is still being voted on!");
		}
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/votekick" };
	}

	//static VoteKick* instance;
protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/votekick " };
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
	bool findGuest(string g) {

		// Get the guest
		string guest = g;
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
	Guest& sender;
	GuestList guests;
	ParsecDSO* _parsec;
	bool _isHost;
	static Stopwatch _stopwatch;
};