#pragma once

#include <iostream>
#include <vector>

using namespace std;

/**
 *  A reduced data structure to hold basic guest data.
 */
class GuestData
{
public:
	/**
	 *  Empty constructor.
	 */
	GuestData()
	{
		name = "";
		userID = 0;
		reason = "";
		fake = false;
		queuedPad = 0;
		ignoredUsers = {};
	}

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userID Unique user ID.
	 */
	GuestData(string name, uint32_t userID)
	{
		set(name, userID, "", 0);
	}
	GuestData(string name, uint32_t userID, string reason)
	{
		set(name, userID, reason, 0);
	}

	void set(string name, uint32_t userID, string reason, int queuedPad)
	{
		this->name = name;
		this->userID = userID;
		this->reason = reason;
	}

	std::string name;
	uint32_t userID;
	std::string reason;
	int queuedPad;
	vector<int> ignoredUsers;
	bool fake;
};




