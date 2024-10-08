#pragma once

#include "../Base/ACommand.h"
#include "../../../GamepadClient.h"
#include "../../../Guest.h"

class ExitQueue : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandFF object
	 *
	 * @param sender
	 * @param gamepadClient
	 * @param hotseat
	 */
	ExitQueue(Guest& sender, GamepadClient& gamepadClient)
		: _sender(sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		if (_sender.queuedPad > 0)
		{
			SetReply(_sender.name + " has left the queue for pad #" + to_string(_sender.queuedPad));
			_gamepadClient.gamepads[_sender.queuedPad - 1]->removeFromQueue(_sender);
			_sender.queuedPad = 0;
			
		}
		else
		{
			SetReply(_sender.name + ", you are not in a queue.");
		}

	}
	/**
	 * @brief Get the prefixes
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/exitqueue" };
	}

protected:
	Guest& _sender;
	GamepadClient& _gamepadClient;
};