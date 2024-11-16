#pragma once

#include "../ACommand.h"
#include "../../Guest.h"
#include "../../Tier.h"
#include "../../Helpers/Stringer.h"
#include "../../Lists/Roles.h"
#include <sstream>

class CommandDefaultMessage : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandDefaultMessage object
	 * 
	 * @param msg
	 * @param sender
	 * @param lastUserID
	 * @param tier
	 * @param isHost
	 */
	CommandDefaultMessage(const char* msg, Guest& sender, uint32_t lastUserID, Tier tier, bool isHost = false)
		: ACommand(msg, sender), _lastUserID(lastUserID), _tier(tier), _isHost(isHost)
	{
		isBotCommand = false;
	}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		std::ostringstream reply;
		if (_sender.userID != _lastUserID)
		{
			static string role = "";
			if (_isHost || _tier == Tier::GOD) role = "HOST  ";
			else if (_tier == Tier::ADMIN || _tier == Tier::MOD) role = "MOD  ";
			else if (_tier == Tier::NOOB) role = "NOOB  ";
			else role = "";

			Role r = GuestRoles::instance.getRole(_sender.userID);
			role = r.messageStarter;

			if (_sender.isValid())
			{
				reply << role << _sender.name;
			}
			else if (_isHost)
			{
				reply << role << "Host";
			}
			else
			{
				reply << role << "Unkown Guest";
			}

			reply << ": ";
		}
		else reply << "\t";
		reply << _msg << "\0";

		_replyMessage = reply.str();
		reply.clear();
		reply.flush();

		return true;
	}

protected:
	uint32_t _lastUserID;
	Tier _tier;
	bool _isHost;
};