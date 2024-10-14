#pragma once

#include "../../../Core/Cache.h"
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
		int queueNum = MetadataCache::getGuestQueueNum(_sender.userID);
		if (queueNum > 0)
		{
			SetReply(_sender.name + " has left the queue for pad #" + to_string(queueNum));
			_gamepadClient.gamepads[queueNum - 1]->removeFromQueue(_sender);
		}
		else
		{
			SetReply(_sender.name + ", you are not in a queue.");
		}
		return true;
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