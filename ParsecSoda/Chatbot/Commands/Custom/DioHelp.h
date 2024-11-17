#pragma once

#include "../../ACommand.h"

using namespace std;

class DioHelp : public ACommand
{
public:
	
	/**
	 * @brief Construct a new DioHelp object
	 *
	 * @param sender
	 */
	DioHelp(const char* msg, Guest& sender)
		: ACommand(msg, sender)
	{} 

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		_replyMessage = Config::cfg.chatbotName + "/queue <num>, /listqueue <num>, /exitqueue, /startcooldown, /tts <text>, /version\n\Mods: /emptyqueue <num>";
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/help", "/commands" };
	}

protected:
};