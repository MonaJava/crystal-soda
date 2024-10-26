#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "matoya.h"
#include "../Models/GuestData.h"
#include "../GuestDataList.h"
#include "../Helpers/PathHelper.h"

class NoobExemptList : GuestDataList {
public:
	NoobExemptList();
	NoobExemptList(const std::vector<GuestData> verifiedUsers);
	bool Add(const GuestData user);
	const bool Remove(const uint32_t userID, function<void(GuestData&)> callback);
	const bool inList(const uint32_t userID);
	vector<GuestData>& getGuests();
	static vector<GuestData> LoadFromFile();
private:
	bool SaveToFile();
};