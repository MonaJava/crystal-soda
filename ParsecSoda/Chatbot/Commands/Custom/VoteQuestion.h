#pragma once

#include "../../ACommand.h"
#include "VoteCommands.h"

class VoteQuestion : public ACommand
{
public:

	std::string usage = "Usage: /poll <message>";

	/**
	 * @brief Construct a new  object
	 *
	 * @param sender
	 */
	VoteQuestion(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Was a question specified?
		/*if (getArgs().size() == 0 && VoteData::instance._isVoting == false) {
			setReply(usage);
			return false;
		}*/
		
		//Polling begin
		if (VoteData::instance._isVoting == false)
		{
			instance = new VoteQuestion(getMsg(), _sender);
			VoteData::instance.setCommand(instance);
			setReply("Poll! " + VoteData::instance._subject + "\nType /yay or /nay to vote!");
			return true;
		}

		int voteDifference = VoteData::instance._yayVotes - VoteData::instance._nayVotes;
		int checkNum = Config::cfg.room.guestLimit / 2;
		//Polling end with yay majority
		if (voteDifference > checkNum)
		{
			setReply(VoteData::instance._subject + " The majority agree!");
			return true;
		}

		//Polling end with nay majority
		if (voteDifference < -checkNum)
		{
			setReply(VoteData::instance._subject + " The majority disagree...");
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
		return vector<const char*> { "/votequestion", "/poll" };
	}

	static VoteQuestion* instance;
protected:
	Guest& _sender;
	
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/votequestion ", "/poll " };
	}
};