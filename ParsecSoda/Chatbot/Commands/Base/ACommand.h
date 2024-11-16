#pragma once

#include "../../../Core/Config.h"
#include <string>
#include <vector>

class ACommand2
{
public:

	/**
	* @brief Is this command a bot command
	*/
	bool isBotCommand = true;

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	virtual bool run() = 0;

	/**
	 * Prefixes the reply with the Chatbot's name
	 */
	void SetReply(const std::string& message) {
		_replyMessage = Config::cfg.chatbotName + message;
		isBotCommand = true;
	}

	/**
	 * @brief Get the reply message
	 * @return The reply message
	 */
	const std::string replyMessage() const { return _replyMessage; }

protected:
	std::string _replyMessage = "";
};