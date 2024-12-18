#pragma once

#include "../../../Core/Cache.h"
#include "../../ACommand.h"

using namespace std;

class DioVersion : public ACommand
{
public:

	/**
	 * @brief Construct a new DioVersion object
	 *
	 * @param sender
	 */
	DioVersion(const char* msg, Guest& sender)
		: ACommand(msg, sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		setReply("v. LIVE AND LEARN");

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/v", "/version", "/latest" };
	}

protected:
};