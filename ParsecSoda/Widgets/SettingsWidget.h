#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../MetadataCache.h"
#include "../Helpers/Debouncer.h"
#include "../Helpers/Countries.h"
#include "../globals/AppIcons.h"
#include "../globals/AppFonts.h"
#include "../globals/AppColors.h"
#include "../globals/AppStyle.h"
#include "../Lists/Roles.h"
#include "TitleTooltipWidget.h"
#include "TooltipWidget.h"
#include "IntRangeWidget.h"

class SettingsWidget
{
public:
	SettingsWidget(Hosting& hosting);
	bool render();
	void renderGeneral();
	void renderChatbot();
	void renderPermissions();
	void renderHotkeys();

private:
	// Dependency injection
	Hosting& _hosting;
	uint32_t DEBOUNCER_TIME_MS = 2000;

	bool _microphoneEnabled = false;
	bool _disableGuideButton = false;
	bool _disableKeyboard = false;
	bool _latencyLimitEnabled = false;
	bool _leaderboardEnabled = false;
	bool _prependPingLimit = false;
	bool _autoIndex = false;
	bool _flashWindow = false;
	bool _sfxEnabled = false;
	bool _bonkEnabled = false;
	bool _hostBonkProof = false;
	bool _parsecLogs = false;
	bool _ipBan = true;
	bool _socketEnabled = true;
	bool _blockVPN = false;
	unsigned int _latencyLimitThreshold = 0;
	unsigned int _theme = 0;
	char _hotkeyCommand[128] = "";
	char _discord[HOST_NAME_LEN] = "";
	char _chatbot[HOST_NAME_LEN] = "";
	char _welcomeMessage[256] = "";

	char _prependRegion[128] = "";

	int32_t _muteTime = 0;
	bool _autoMute;
	int32_t _autoMuteTime = 0;
	int32_t _socketPort = 0;
	bool _saveChat;
	bool _hotkeyBB;
	bool _hotkeyLock;
	bool _showHotkeyForm;

	bool _guestBB = false;
	bool _vipBB = false;
	bool _modBB = false;
	
	bool _guestSFX = false;
	bool _vipSFX = false;
	bool _modSFX = false;

	bool _guestControls = false;
	bool _vipControls = false;
	bool _modControls = false;

	int _noobNum = 160;
	bool _kickNoob = false;
	bool _limitNoob = true;

	bool _BB = false;
	bool _SFX = false;
	bool _controls = false;
	bool _kick = false;
	bool _limit = false;

	string themes[5] = { "Midnight", "Parsec Soda", "Parsec Soda V", "Mini", "Arcade" };
	vector<Role> rolelist = vector<Role>();
	Role _displayRole = Role();
	char _roleName[128] = "";
	char _messageStarter[128] = "";
	char _commandPrefix[128] = "";
	Countries _countries;

	int testNum = 1;
	string testWord = "noob";
};