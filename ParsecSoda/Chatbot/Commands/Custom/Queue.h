#pragma once

#include "../Base/ACommandIntegerArg.h"
#include "../../../GamepadClient.h"

class Queue : public ACommandIntegerArg
{
public:

	/**
	 * @brief Construct a new CommandSwap object
	 *
	 * @param msg
	 * @param sender
	 * @param gamepadClient
	 */
	Queue(const char* msg, Guest& sender, GamepadClient& gamepadClient)
		: ACommandIntegerArg(msg, internalPrefixes()), _sender(sender), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		if (!ACommandIntegerArg::run()) {
			SetReply("Usage: /queue <integer in range [1, 4]>\nExample: /queue 4\0");
			return false;
		}

		GamepadClient::PICK_REQUEST result = _gamepadClient.pick(_sender, _intArg - 1);

		bool rv = false;
		std::ostringstream reply;
		AGamepad* pad = nullptr;
		

		switch (result)
		{
		case GamepadClient::PICK_REQUEST::OK:
			reply
				<< Config::cfg.chatbotName + "Gamepad " << _intArg << " was given to " << _sender.name << "\t(#" << _sender.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			rv = true;
			break;
		case GamepadClient::PICK_REQUEST::DISCONNECTED:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", gamepad " << _intArg << " is offline.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::SAME_USER:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", you have that gamepad already.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::TAKEN:
			if (_sender.queuedPad > 0)
			{
				reply
					<< Config::cfg.chatbotName << _sender.name << ", you've already queued for pad #" << _sender.queuedPad << "\n"
					<< "Type /exitqueue to leave the queue.\n"
					<< "\t\tType !pads to see the gamepad list.\0";
			}
			else
			{
				reply
					<< Config::cfg.chatbotName << "Gamepad " << _intArg << " has been reserved by " << _sender.name << "\n"
					<< "\t\tType !pads to see the gamepad list.\0";
				_gamepadClient.gamepads[_intArg - 1]->addToQueue(_sender);
				_sender.queuedPad = _intArg;
			}
			break;
		case GamepadClient::PICK_REQUEST::EMPTY_HANDS:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", you don't need to queue for an available pad.\n"
				<< "\t\tPress any face button (A, B, X, Y) to receive a random gamepad (if available).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::LIMIT_BLOCK:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", your current gamepad limit is set to 0.\0";
			break;
		case GamepadClient::PICK_REQUEST::OUT_OF_RANGE:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", your gamepad index is wrong (valid range is [1, 4]).\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::PUPPET:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", puppet master is handling that gamepad.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::RESERVED:
			if (_sender.queuedPad > 0)
			{
				reply
					<< Config::cfg.chatbotName << _sender.name << ", you've already queued for pad #" << _sender.queuedPad << "\n"
					<< "Type /exitqueue to leave the queue.\n"
					<< "\t\tType !pads to see the gamepad list.\0";
			}
			else
			{
				pad = _gamepadClient.gamepads[_intArg - 1];
				reply
					<< Config::cfg.chatbotName << _sender.name << " has been added to the queue for pad #" << _intArg << "\n"
					<< Config::cfg.chatbotName << " Waiting list:\n";
				_gamepadClient.gamepads[_intArg - 1]->addToQueue(_sender);
				_sender.queuedPad = _intArg;


				for (size_t i = 0; i < pad->getQueue().size(); i++)
				{
					reply << i + 1 << ": " << pad->getQueue()[i].name << "  (#" << pad->getQueue()[i].userID << ")\n";
				}
				reply << "\t\tType !pads to see the gamepad list.\0";
			}

				 
			break;
		default:
			break;
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
		return vector<const char*> { "/queue" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "/queue ", };
	}
	string _msg;
	Guest& _sender;
	GamepadClient& _gamepadClient;
};