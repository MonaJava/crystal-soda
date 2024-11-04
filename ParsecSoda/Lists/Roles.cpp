#include "Roles.h"
#include "../Helpers/Stringer.h"
Roles Roles::r;
GuestRoles GuestRoles::instance;

/*/**
 * @brief Load the role list from file
 */

map<string, Role> Roles::LoadFromFile() {
    map<string, Role> result;

    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "roles.json";

        if (MTY_FileExists(filepath.c_str())) {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++) {
                const MTY_JSON* role = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[128] = "";
                char messageStarter[128] = "";
                char commandPrefix[128] = "";
                char key[128] = "";

                bool nameSuccess = MTY_JSONObjGetString(role, "name", name, 128);
                bool messageStarterSuccess = MTY_JSONObjGetString(role, "messageStarter", messageStarter, 128);
                bool commandPrefixSuccess = MTY_JSONObjGetString(role, "commandPrefix", commandPrefix, 128);
                bool keySuccess = MTY_JSONObjGetString(role, "key", key, 128);

                if (nameSuccess && messageStarterSuccess && commandPrefixSuccess && keySuccess) {
                    result[key] = Role(name, messageStarter, commandPrefix, key);
                }
            }
            MTY_JSONDestroy(&json);
        }
        else
            result = list;
    }

    return result;
}

/**
 * @brief Save the role list to file
 */
bool Roles::SaveToFile() {
    string dirPath = PathHelper::GetConfigPath();
    if (!dirPath.empty()) {
        string filepath = dirPath + "roles.json";

        MTY_JSON* json = MTY_JSONArrayCreate();
        
        
        map<string, Role>::iterator gi = r.list.begin();
        for (; gi != r.list.end(); ++gi) {
            MTY_JSON* role = MTY_JSONObjCreate();

            MTY_JSONObjSetString(role, "name", (*gi).second.name.c_str());
            MTY_JSONObjSetString(role, "messageStarter", (*gi).second.messageStarter.c_str());
            MTY_JSONObjSetString(role, "commandPrefix", (*gi).second.commandPrefix.c_str());
            MTY_JSONObjSetString(role, "key", (*gi).first.c_str());
            MTY_JSONArrayAppendItem(json, role);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}


//------------

void GuestRoles::setRole(uint32_t userID, Role role)
{
    vector<GuestRole>::iterator it;
    for (it = _guestRoles.begin(); it != _guestRoles.end(); ++it)
    {
        if (userID == (*it).userID)
        {
            if (role.key != "guest")
            {
                (*it).role = role;
            }
            else
            {
                _guestRoles.erase(it);
            }

            saveRoles();

            return;
        }
    }

    if (role.key != "guest")
    {
        _guestRoles.push_back(GuestRole(userID, role));
        saveRoles ();
    }
}

Role GuestRoles::getRole(uint32_t userID) {
    vector<GuestRole>::iterator it;
    for (it = _guestRoles.begin(); it != _guestRoles.end(); ++it)
    {
        if ((*it).userID == userID)
        {
            return (*it).role;
        }
    }

    return Roles::r.list["guest"];
}

void GuestRoles::loadRoles() {
    _guestRoles = MetadataCache::loadGuestRoles();
}

bool GuestRoles::saveRoles() {
    return MetadataCache::saveGuestRoles(_guestRoles);
}