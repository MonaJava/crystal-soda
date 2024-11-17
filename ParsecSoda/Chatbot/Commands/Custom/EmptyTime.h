#pragma once


#include "../../../Modules/Hotseat.h"
#include "../../../GuestList.h"
#include "../../ACommand.h"

class EmptyTime : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandAFK object
	 *
	 * @param guests
	 * @param padClient
	 */
	EmptyTime(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		Hotseat::instance.seatUser(_sender.userID, _sender.name);
		Hotseat::instance.reduceUserPlaytime(_sender.userID, 999);
		setReply(_sender.name + " has given up his remaining playtime\0");
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/emptyplaytime", "/startcooldown" };
	}
protected:
	Guest& _sender;
};