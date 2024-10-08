#pragma once

#include "../Base/ACommandIntegerArg.h"
#include "../../../GamepadClient.h"

class EmptyQueue : public ACommandIntegerArg
{
public:

	/**
	 * @brief Construct a new CommandSwap object
	 *
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	EmptyQueue(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommandIntegerArg(msg, internalPrefixes()), _sender(sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		vector<Guest>& queue = _gamepadClient.gamepads[_intArg - 1]->getQueue();
		if (!ACommandIntegerArg::run()) {
			SetReply("Usage: /emptyqueue <integer in range [1, 4]>\nExample: /emptyqueue 4\0");
			return false;
		}
		AGamepad* pad = nullptr;
		if (_intArg > 0 && _intArg < _gamepadClient.gamepads.size())
		{
			_gamepadClient.gamepads[_intArg - 1]->eraseQueue();
			SetReply("The queue for pad #" + to_string(_intArg) + " has been cleared.\0");
		}
		else
		{
			SetReply(_sender.name + ", your gamepad index is wrong (valid range is [1, 4]).\n"
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