#include "NoobExemptList.h"
#include "../Helpers/Stringer.h"

/**
 * @brief Construct a new NoobExemptList List:: NoobExemptList List object
 */
NoobExemptList::NoobExemptList() {
}

/**
 * @brief Construct a new NoobExemptList List:: NoobExemptList List object
 * 
 * @param verifiedUsers 
 */
NoobExemptList::NoobExemptList(std::vector<GuestData> verifiedUsers)
	: GuestDataList(verifiedUsers) { }

/**
 * @brief Add a user to the list
 * 
 * @param user 
 * @return true 
 * @return false 
 */
bool NoobExemptList::Add(GuestData user) {
	bool added = GuestDataList::add(user);
	if (added) {
		SaveToFile();
	}
	return added;
}

/**
 * @brief Remove a user from the list
 * 
 * @param userID 
 * @param callback 
 * @return true 
 * @return false 
 */
const bool NoobExemptList::Remove(const uint32_t userID, function<void(GuestData&)> callback) {
	bool found = GuestDataList::pop(userID, callback);
	if (found) {
		SaveToFile();
	}
	return found;
}

/**
 * @brief Check if a user is in the list
 * 
 * @param userID 
 * @return const bool 
 */
const bool NoobExemptList::inList(const uint32_t userID) {
	return find(userID);
}

/**
 * @brief Get the list of verified guests
 * 
 * @return vector<GuestData>& 
 */
vector<GuestData>& NoobExemptList::getGuests() {
	return GuestDataList::getGuests();
}

/**
 * @brief Load the verified users list from file
 */
vector<GuestData> NoobExemptList::LoadFromFile() {
	vector<GuestData> result;

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "noobexempt.json";

        if (MTY_FileExists(filepath.c_str())) {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++) {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[128] = "";
                uint32_t userID = 0;
                bool nameSuccess = MTY_JSONObjGetString(guest, "name", name, 128);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (nameSuccess && userIDSuccess) {
                    result.push_back(GuestData(name, userID));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestData a, const GuestData b) {
                return a.userID < b.userID;
                });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

/**
 * @brief Save the verified users list to file
 */
bool NoobExemptList::SaveToFile() {
	string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "noobexempt.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestData>::iterator gi = _guests.begin();
        for (; gi != _guests.end(); ++gi) {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetString(guest, "name", (*gi).name.c_str());
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}