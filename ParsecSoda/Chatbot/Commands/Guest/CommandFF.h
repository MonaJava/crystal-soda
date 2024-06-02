#pragma once

#include "../Base/ACommand.h"
#include "../../../GamepadClient.h"
#include "../../../Guest.h"

class CommandFF : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandFF object
	 * 
	 * @param sender 
	 * @param gamepadClient 
	 * @param hotseat 
	 */
	CommandFF(Guest &sender, GamepadClient &gamepadClient, Hotseat &hotseat)
		: _sender(sender), _gamepadClient(gamepadClient), _droppedPadCount(0), _hotseat(hotseat)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		if (Config::cfg.hotseat.enabled) {
			Hotseat::instance.pauseUser(_sender.userID);
		}

		_droppedPadCount = _gamepadClient.onQuit(_sender);
		if (_droppedPadCount > 0) {
			SetReply(_sender.name + " has dropped " + std::to_string(_droppedPadCount) + " gamepad(s)!\0");
		} else {
			SetReply(_sender.name + " has no gamepads to drop.\0");
		}

		return true;
	}

	/**
	 * @brief Get the dropped pad count
	 * @return int 
	 */
	int droppedPadCount() {
		return _droppedPadCount;
	}

	/**
	 * @brief Get the prefixes
	 * @return vector<const char*> 
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!ff", "!drop", "!quit" };
	}

protected:
	Guest& _sender;
	GamepadClient &_gamepadClient;
	Hotseat& _hotseat;
	int _droppedPadCount;
};