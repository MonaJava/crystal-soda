#pragma once

#include "../../ACommand.h"
#include "../../../GamepadClient.h"

class GivePad : public ACommand
{
public:

	std::string usage = "Usage: /give <username> <slot>\nExample: /give obama 1\0";

	/**
	 * @brief Construct a new CommandExtend object
	 *
	 * @param msg
	 * @param sender
	 * @param guests
	 * @param host
	 */
	GivePad(const char* msg, Guest& sender, GuestList& guests, Guest& host, GamepadClient& gamepadClient)
		: ACommand(msg, sender), _host(host), guests(guests), _gamepadClient(gamepadClient)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		int nGamepads = _gamepadClient.gamepads.size();

		if (nGamepads == 0) {
			setReply("No gamepads available.\0");
			return false;
		}

		// Was a guest specified?
		if (getArgs().size() < 2) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Can't find the guest!\0");
			return false;
		}

		int slot = 0;
		try {
			slot = std::stoi(getArgs()[1]);
		}
		catch (std::invalid_argument) {
			setReply(usage);
			return false;
		}




		// In range [1, nGamepads]
		if (slot < 1 || slot > nGamepads) {
			setReply("Usage: !swap <integer in range [1, " + std::to_string(nGamepads) + "]>\nExample: !swap 4\0");
			return false;
		}

		GamepadClient::PICK_REQUEST result = _gamepadClient.pick(target, slot - 1);
		AGamepad* pad = _gamepadClient.gamepads[slot - 1];

		bool rv = false;
		std::ostringstream reply;
		Role role = Roles::r.list[GuestRoles::instance.getRole(target.userID).key];

		switch (result)
		{
		case GamepadClient::PICK_REQUEST::OK:
			reply
				<< Config::cfg.chatbotName + "Gamepad " << slot << " was given to " << target.name << "\t(#" << target.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			rv = true;
			break;
		case GamepadClient::PICK_REQUEST::DISCONNECTED:
			reply
				<< Config::cfg.chatbotName << _sender.name << ", gamepad " << slot << " is offline.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::SAME_USER:
			reply
				<< Config::cfg.chatbotName << target.name << " has that gamepad already.\n"
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		case GamepadClient::PICK_REQUEST::EMPTY_HANDS:
			reply
				<< Config::cfg.chatbotName + "Gamepad " << slot << " was given to " << target.name << "\t(#" << target.userID << ")\n"
				<< "\t\tType !pads to see the gamepad list.\0";
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
		case GamepadClient::PICK_REQUEST::TAKEN:
		case GamepadClient::PICK_REQUEST::LIMIT_BLOCK:
		case GamepadClient::PICK_REQUEST::ROLE_BLOCK:
			reply
				<< Config::cfg.chatbotName << "Gamepad " << slot << " was forcefully given to " << target.name << "\t(#" << target.userID << ")\n";
			if (pad->owner.guest.isValid()) {
				reply
					<< pad->owner.guest.name << "\t(#" << pad->owner.guest.userID << ") has been booted from the gamepad.\n";
			}
			if (!Config::cfg.hotseat.enabled || Hotseat::instance.checkUser(target.userID, target.name)) {

				if (Config::cfg.hotseat.enabled) {

					if (pad->owner.guest.isValid()) {
						Hotseat::instance.pauseUser(pad->owner.guest.userID);
					}

					Hotseat::instance.seatUser(target.userID, target.name);
				}

				pad->owner.guest.copy(target);
			}
			reply
				<< "\t\tType !pads to see the gamepad list.\0";
			break;
		default:
			break;
		}

		_replyMessage = reply.str();
		return false;
	




		return false;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return std::vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/give" };
	}

protected:
	static vector<const char*> internalPrefixes()
	{
		return vector<const char*> { "/give " };
	}

	/**
	* Get the guest referenced in the command. Returns nullptr
	* if no guest is found
	*
	* @param guestList The guest list
	*/
	bool findGuest() {

		// Get the guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}

		try {
			uint32_t id = stoul(guest);
			vector<Guest>::iterator i;
			for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
				if ((*i).userID == id) {
					target = *i;
					return true;
				}
			}
		}
		catch (const std::exception&) {
			bool found = guests.find(guest, &target);
			if (found) {
				return true;
			}
		}

		return false;
	}

	Guest target;
	GuestList guests;

	Guest& _host;
	GamepadClient& _gamepadClient;
};