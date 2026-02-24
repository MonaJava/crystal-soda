#pragma once

#include "../../ACommand.h"
#include "parsec.h"

class Ignore : public ACommand
{
public:

	std::string usage = "Usage: !ignore <guest>\0";

	/**
	 * @brief Construct a new CommandPing object
	 *
	 * @param msg
	 * @param sender
	 * @param guests
	 * @param host
	 */
	Ignore(const char* msg, Guest& sender, GuestList& guests, Guest& host, GuestList& guestList)
		: ACommand(msg, sender), _host(host), guests(guestList)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (!findGuest()) {
			setReply("Can't find the guest you want to ignore!\0");
			return false;
		}

		if (MetadataCache::isIgnored(_sender.userID, target.userID)) {
			MetadataCache::removeFromIgnored(_sender.userID, target.userID);
			setReply(target.name + " has been unignored!\0");
		}
		else
		{
			MetadataCache::addToIgnored(_sender.userID, target.userID);
			setReply(target.name + " has been ignored!\0");
		}
		return false;
	}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/ignore" };
	}

protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/ignore " };
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
	Guest& _host;
	GuestList guests;
};