#pragma once

#include "../../ACommand.h"
#include "../../../GamepadClient.h"

class EmptyQueue : public ACommand
{
public:
	/**
	 * Example of how to use the command
	 */
	std::string usage = "/emptyqueue <int>";

	/**
	 * @brief Construct a new CommandSwap object
	 *
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	EmptyQueue(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _sender(sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		if (getArgs().size() == 0) {
			setReply("Usage: /emptyqueue <int>\0");
			return false;
		}

		
		int _intArg;
		_intArg = std::stoi(getArgs()[0]);
		vector<Guest>& queue = _gamepadClient.gamepads[_intArg - 1]->getQueue();
		
		AGamepad* pad = nullptr;
		if (_intArg > 0 && _intArg < _gamepadClient.gamepads.size())
		{
			_gamepadClient.gamepads[_intArg - 1]->eraseQueue();
			setReply("The queue for pad #" + to_string(_intArg) + " has been cleared.\0");
		}
		else
		{
			setReply(_sender.name + ", your gamepad index is wrong (valid range is [1, 4]).\n"
				+ "\t\tType !pads to see the gamepad list.\0");
		}
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/emptyqueue" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "/emptyqueue ", };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
};