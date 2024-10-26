#pragma once

#include "../Base/ACommand.h"

using namespace std;

class DioHelp : public ACommand
{
public:

	/**
	 * @brief Construct a new DioHelp object
	 *
	 * @param sender
	 */
	DioHelp()
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {
		_replyMessage = Config::cfg.chatbotName + "/queue <num>, /listqueue <num>, /exitqueue, /startcooldown, /tts <text>\n\Mods: /unnoob <guest>, /emptyqueue <num>";
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