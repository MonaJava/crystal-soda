#pragma once

#include "../../ACommand.h"
#include "../../../GamepadClient.h"

class ListQueue : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandSwap object
	 *
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	ListQueue(const char* msg, Guest& sender, GamepadClient& gamepadClient)
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
			setReply("Usage: /listqueue <int>\0");
			return false;
		}

		bool rv = false;
		std::ostringstream reply;

		int _intArg;
		_intArg = std::stoi(getArgs()[0]);

		if (_intArg > 0 && _intArg <= _gamepadClient.gamepads.size())
		{
			reply
				<< Config::cfg.chatbotName << "Pad #" << _intArg << " Waiting list : \n";

			vector<Guest>& queue = _gamepadClient.gamepads[_intArg - 1]->getQueue();

			for (size_t i = 0; i < queue.size(); i++)
			{
				reply << i + 1 << ": " << queue[i].name << "  (#" << queue[i].userID << ")\n";
			}
			reply << "\t\tType !pads to see the gamepad list.\0";
		}
		else
		{
			reply
				<< Config::cfg.chatbotName << _sender.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
		}

		_replyMessage = reply.str();
		return rv;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/listqueue" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "/listqueue ", };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
};