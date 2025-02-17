#pragma once

#include "../../ACommand.h"
#include "../../../Modules/Hotseat.h"
#include "../../../GamepadClient.h"

using namespace std;

class CommandHotseat : public ACommand
{
public:

	/**
	 * @brief Construct a new CommandHotseat object
	 * 
	 * @param sender
	 * @param hotseat
	 */
	CommandHotseat(const char* msg, Guest& sender, Hotseat& hotseat, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _hotseat(hotseat), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 * 
	 * @return true
	 * @return false
	 */
	bool run() override {
		if (Config::cfg.hotseat.enabled) {
			setReply("Hotseat has been disabled.");
			Config::cfg.hotseat.enabled = false;
			Hotseat::instance.stop();
		}
		else {
			setReply("Hotseat has been enabled.");
			Config::cfg.hotseat.enabled = true;
			Hotseat::instance.start();
			for (size_t i = 0; i < _gamepadClient.gamepads.size(); ++i)
			{
				if (_gamepadClient.gamepads[i]->isOwned())
				{
					Guest guest = _gamepadClient.gamepads[i]->owner.guest;
					if (Hotseat::instance.checkUser(guest.userID, guest.name)) {
						Hotseat::instance.seatUser(guest.userID, guest.name);
					}
				}
			}
		}

			return true;

		}

	/**
	 * @brief Get the prefixes object
	 * 
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!hotseat", "!hotseatoff", "!hotseaton" };
	}

protected:

	void disableHotseat() {
		_hotseat.stop();
		Config::cfg.hotseat.enabled = false;
		Config::cfg.Save();
		setReply("Hotseat mode disabled");
	}

	void enableHotseat() {
		_hotseat.start();
		Config::cfg.hotseat.enabled = true;
		Config::cfg.Save();
		setReply("Hotseat mode enabled");
	}
	Hotseat& _hotseat;
	GamepadClient& _gamepadClient;
};