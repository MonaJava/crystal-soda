#pragma once

#include "../../../Core/Cache.h"
#include "../Base/ACommand.h"

using namespace std;

class DioVersion : public ACommand
{
public:

	/**
	 * @brief Construct a new DioVersion object
	 *
	 * @param sender
	 */
	DioVersion()
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		SetReply("v.1132024");

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