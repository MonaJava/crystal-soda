#include "Hosting.h"

using namespace std;

#if defined(_WIN32)
	#if !defined(BITS)
		#define BITS 64
	#endif
	#if (BITS == 64)
		#define SDK_PATH "./parsec.dll"
	#else
		#define SDK_PATH "./parsec32.dll"
	#endif
#endif

// ============================================================
// 
//  PUBLIC
// 
// ============================================================

Hosting::Hosting()
{

	// Create a random 8 character string
	_roomToken = "";
	
	_hostConfig = EMPTY_HOST_CONFIG;
	MetadataCache::loadPreferences();
	setHostConfig(
		"",
		"",
		Config::cfg.room.guestLimit,
		false,
		Config::cfg.room.secret
	);
	setHostVideoConfig(Config::cfg.video.fps, Config::cfg.video.bandwidth);

	_sfxList.init("./SFX/custom/_sfx.json");
	
	_tierList.loadTiers();
	_tierList.saveTiers();

	vector<GuestData> banned = MetadataCache::loadBannedUsers();
	_banList = BanList(banned);

	vector<GuestData> modded = MetadataCache::loadModdedUsers();
	_modList = ModList(modded);

	vector<GuestData> vip = MetadataCache::loadVIPUsers();
	_vipList = VIPList(vip);

	vector<GameData> games = MetadataCache::loadGamesList();
	_gamesList = GameDataList(games);

	_parsec = nullptr;

	SDL_Init(SDL_INIT_JOYSTICK);
	_masterOfPuppets.init(_gamepadClient);
	_masterOfPuppets.start();

	_latencyLimitEnabled = Config::cfg.room.latencyLimit;
	_latencyLimitThreshold = Config::cfg.room.latencyLimitThreshold;
	_disableGuideButton = Config::cfg.input.disableGuideButton;
	_disableKeyboard = Config::cfg.input.disableKeyboard;
	_lockedGamepad.bLeftTrigger = Config::cfg.input.lockedGamepadLeftTrigger;
	_lockedGamepad.bRightTrigger = Config::cfg.input.lockedGamepadRightTrigger;
	_lockedGamepad.sThumbLX = Config::cfg.input.lockedGamepadLX;
	_lockedGamepad.sThumbLY = Config::cfg.input.lockedGamepadLY;
	_lockedGamepad.sThumbRX = Config::cfg.input.lockedGamepadRX;
	_lockedGamepad.sThumbRY = Config::cfg.input.lockedGamepadRY;
	_lockedGamepad.wButtons = Config::cfg.input.lockedGamepadButtons;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void Hosting::applyHostConfig() {
	
	if (isRunning()){
		ParsecHostSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
	}
}

void Hosting::broadcastChatMessage(string message)
{
	vector<Guest> guests = _guestList.getGuests();
	vector<Guest>::iterator gi;

	for (gi = guests.begin(); gi != guests.end(); ++gi)
	{
		ParsecHostSendUserData(_parsec, (*gi).id, HOSTING_CHAT_MSG_ID, message.c_str());
	}
}

void Hosting::init() {
	
	_parsecStatus = ParsecInit(NULL, NULL, (char *)SDK_PATH, &_parsec);
	_dx11.init();
	_gamepadClient.setParsec(_parsec);
	_gamepadClient.init();

	MetadataCache::Preferences preferences = MetadataCache::loadPreferences();

	_createGamepadsThread = thread([&]() {
		_gamepadClient.createAllGamepads();
		_createGamepadsThread.detach();
		_macro.init(_gamepadClient, _guestList);
		_tournament.init(*_parsec, _guestList, _gamepadClient, _chatLog, _macro);
	});

	audioOut.fetchDevices();
	vector<AudioOutDevice> audioOutDevices = audioOut.getDevices();
	if (preferences.audioOutputDevice >= audioOutDevices.size()) {
		preferences.audioOutputDevice = 0;
	}
	audioOut.setOutputDevice(preferences.audioOutputDevice);		// TODO Fix leak in setOutputDevice
	audioOut.captureAudio();
	audioOut.volume = 0.3f;
	audioOut.setFrequency((Frequency)Config::cfg.audio.speakersFrequency);

	vector<AudioInDevice> audioInputDevices = audioIn.listInputDevices();
	if (preferences.audioInputDevice >= audioInputDevices.size()) {
		preferences.audioInputDevice = 0;
	}
	AudioInDevice device = audioIn.selectInputDevice(Config::cfg.audio.inputDevice);
	audioIn.init(device);
	audioIn.volume = 0.8f;
	audioIn.setFrequency((Frequency)Config::cfg.audio.micFrequency);

	preferences.isValid = true;
	MetadataCache::savePreferences(preferences);
	_parsecSession.loadSessionCache();

	fetchAccountData();

	_chatBot = new ChatBot(
		audioIn, audioOut, _banList, _dx11, _modList, _vipList,
		_gamepadClient, _guestList, _guestHistory, _parsec,
		_hostConfig, _parsecSession, _sfxList, _tierList, 
		_macro, _isRunning, _host, _hotseat, _tournament
	);

	CommandBonk::init();

	// A way to test chat and guest managment features
	// without hosting the room
	_isTestMode = false;
	if (_isTestMode) {

		// Add (n) number of fake guests
		//srand(time(NULL));
		//addFakeGuests(8);
		
	}
	
}

void Hosting::release() {
	stopHosting();
	while (_isRunning)
	{
		Sleep(5);
	}
	_dx11.clear();
	_gamepadClient.release();
	_masterOfPuppets.stop();
}

bool Hosting::isReady() {
	
	return _parsecStatus == PARSEC_OK;
}

bool Hosting::isRunning() {
	return _isTestMode || _isRunning;
}

bool& Hosting::isGamepadLock()
{
	return _gamepadClient.lock;
}

bool& Hosting::isGamepadLockButtons()
{
	return _gamepadClient.lockButtons;
}

Guest& Hosting::getHost()
{
	return _host;
}

ParsecSession& Hosting::getSession()
{
	return _parsecSession;
}

void Hosting::fetchAccountData(bool sync)
{
	_host.name = "Host";
	_host.status = Guest::Status::INVALID;
	if (isReady()) {
		if (sync) {
			_parsecSession.fetchAccountDataSync(&_host);
		}
		else {
			_parsecSession.fetchAccountData(&_host);
		}
	}
}

ParsecHostConfig& Hosting::getHostConfig()
{
	return _hostConfig;
}

DX11& Hosting::getDX11()
{
	return _dx11;
}

ChatBot* Hosting::getChatBot()
{
	return _chatBot;
}

vector<string>& Hosting::getMessageLog()
{
	return _chatLog.getMessageLog();
}

vector<string>& Hosting::getCommandLog()
{
	return _chatLog.getCommandLog();
}

GuestList& Hosting::getGuestList() {
	return _guestList;
}

vector<Guest>& Hosting::getGuests() {
	return _guestList.getGuests();
}

vector<Guest>& Hosting::getPlayingGuests() {
	return _guestList.getPlayingGuests();
}

vector<Guest>& Hosting::getRandomGuests() {
	return _guestList.getRandomGuests();
}

Guest& Hosting::getGuest(uint32_t id) {
	
	// For each guest in the guest list
	for (Guest& guest : _guestList.getGuests()) {

		// If the guest ID matches the target ID
		if (guest.id == id) {

			// Return the guest
			return guest;
		}
	}
	
}

int Hosting::getGuestIndex(uint32_t id) {

	// For each guest in the guest list
	for (int i = 0; i < _guestList.getGuests().size(); i++) {

		// If the guest ID matches the target ID
		if (_guestList.getGuests()[i].userID == id) {

			// Return the guest
			return i;
		}
	}

	return -1;

}

bool Hosting::guestExists(uint32_t id) {
	for (Guest& guest : _guestList.getGuests()) {
		if (guest.id == id) {
			return true;
		}
	}
	return false;
}

vector<Guest>& Hosting::getGuestsAfterGuest(uint32_t targetGuestID, int count, bool ignoreSpectators) {
	return _guestList.getGuestsAfterGuest(targetGuestID, count, ignoreSpectators);
}

vector<GuestData>& Hosting::getGuestHistory()
{
	return _guestHistory.getGuests();
}

MyMetrics Hosting::getMetrics(uint32_t id)
{
	return _guestList.getMetrics(id);
}

BanList& Hosting::getBanList()
{
	return _banList;
}

ModList& Hosting::getModList()
{
	return _modList;
}

VIPList& Hosting::getVIPList()
{
	return _vipList;
}

GameDataList& Hosting::getGameList()
{
	return _gamesList;
}

vector<AGamepad*>& Hosting::getGamepads() {
	return _gamepadClient.gamepads;
}

GamepadClient& Hosting::getGamepadClient()
{
	return _gamepadClient;
}

MasterOfPuppets& Hosting::getMasterOfPuppets()
{
	return _masterOfPuppets;
}

Overlay& Hosting::getOverlay() {
	return _overlay;
}

const char** Hosting::getGuestNames()
{
	return _guestList.guestNames;
}

void Hosting::toggleGamepadLock()
{
	_gamepadClient.toggleLock();
}

void Hosting::toggleGamepadLockButtons()
{
	_gamepadClient.toggleLockButtons();
}

void Hosting::setGameID(string gameID)
{
	try
	{
		strcpy_s(_hostConfig.gameID, gameID.c_str());
	}
	catch (const std::exception&) {}
}

void Hosting::setMaxGuests(uint8_t maxGuests)
{
	_hostConfig.maxGuests = maxGuests;
}

void Hosting::setHostConfig(string roomName, string gameId, uint8_t maxGuests, bool isPublicRoom)
{
	setRoomName(roomName);
	setGameID(gameId);
	setMaxGuests(maxGuests);
	setPublicRoom(isPublicRoom);
}

void Hosting::setHostConfig(string roomName, string gameId, uint8_t maxGuests, bool isPublicRoom, string secret)
{
	setRoomName(roomName);
	setGameID(gameId);
	setMaxGuests(maxGuests);
	setPublicRoom(isPublicRoom);
	setRoomSecret(secret);
}

void Hosting::setHostVideoConfig(uint32_t fps, uint32_t bandwidth)
{
	_hostConfig.video->encoderFPS = fps;
	_hostConfig.video->encoderMaxBitrate = bandwidth;
	Config::cfg.video.fps = fps;
	Config::cfg.video.bandwidth = bandwidth;
}

void Hosting::setPublicRoom(bool isPublicRoom)
{
	_hostConfig.publicGame = isPublicRoom;
}

void Hosting::setRoomName(string roomName)
{
	try
	{
		strcpy_s(_hostConfig.name, roomName.c_str());
	}
	catch (const std::exception&) {}
}

void Hosting::setRoomSecret(string secret)
{

	try
	{
		strcpy_s(_hostConfig.secret, secret.c_str());
	}
	catch (const std::exception&) {}
}

/// <summary>
/// Starts hosting the Parsec session.
/// </summary>
void Hosting::startHosting() {

	if (!_isRunning) {
		_isRunning = true;
		initAllModules();

		try
		{
			if (_parsec != nullptr)
			{
				_mediaThread = thread ( [this]() {liveStreamMedia(); } );
				_inputThread = thread ([this]() {pollInputs(); });
				_eventThread = thread ([this]() {pollEvents(); });
				_latencyThread = thread([this]() {pollLatency(); });
				_smashSodaThread = thread([this]() { pollSmashSoda(); });
				_mainLoopControlThread = thread ([this]() {mainLoopControl(); });

				// Start the chat automoderator
				AutoMod::instance.Start();

				// Hotseat mode
				if (Config::cfg.hotseat.enabled) {
					Hotseat::instance.Start();
				}

				// Overlay
				if (Config::cfg.overlay.enabled) {
					_overlay.start();
				}

			}
		}
		catch (const exception&)
		{}
	}

	bool debug = true;
}

/// <summary>
/// Stops hosting the Parsec session.
/// </summary>
void Hosting::stopHosting() {

	// Stop the chat automoderator
	AutoMod::instance.Stop();

	// Remove post on Soda Arcade
	if (!Config::cfg.room.privateRoom) {
		Arcade::instance.deletePost();
	}

	_isRunning = false;
	_guestList.clear();

	// Stop hotseat mode
	if (Config::cfg.hotseat.enabled) {
		Hotseat::instance.Stop();
	}

	// Disable overlay
	_overlay.stop();

	// Stop kiosk mode
	if (Config::cfg.kioskMode.enabled) {
		ProcessMan::instance.stop();
	}

}

void Hosting::stripGamepad(int index)
{
	_gamepadClient.clearOwner(index);
}

void Hosting::setOwner(AGamepad& gamepad, Guest newOwner, int padId)
{
	bool found = _gamepadClient.findPreferences(newOwner.userID, [&](GamepadClient::GuestPreferences& prefs) {
		gamepad.setOwner(newOwner, padId, prefs.mirror);
	});

	if (!found) {
		gamepad.setOwner(newOwner, padId, false);
	}
}

void Hosting::handleMessage(const char* message, Guest& guest, bool isHost, bool isHidden, bool outside) {
	
	// Has the user been muted?
	if (AutoMod::instance.isMuted(guest.userID)) {
		return;
	}

	ACommand* command = _chatBot->identifyUserDataMessage(message, guest, isHost);
	command->run();

	// Non-blocked default message
	if (!isFilteredCommand(command))
	{
		Tier tier = _tierList.getTier(guest.userID);

		CommandDefaultMessage defaultMessage(message, guest, _chatBot->getLastUserId(), tier, isHost);
		defaultMessage.run();
		_chatBot->setLastUserId(guest.userID);

		if (!defaultMessage.replyMessage().empty() && !isHidden) {
			broadcastChatMessage(defaultMessage.replyMessage());
			
			string adjustedMessage = defaultMessage.replyMessage();
			Stringer::replacePatternOnce(adjustedMessage, "%", "%%");
			_chatLog.logMessage(adjustedMessage);
			_overlay.sendChatMessage(guest, message);

			// Record last message
			AutoMod::instance.RecordMessage(guest.userID, guest.name, message);
		}
	}

	// Chatbot's command reply
	if (!command->replyMessage().empty() && command->type() != COMMAND_TYPE::DEFAULT_MESSAGE) {
		broadcastChatMessage(command->replyMessage());
		_chatLog.logCommand(command->replyMessage());
		_chatBot->setLastUserId();
		_overlay.sendLogMessage(command->replyMessage());
	}

	delete command;

}

/// <summary>
/// This handles muted guests and auto mutes anybody spamming chat.
/// </summary>
/// <param name="message">The message sent.</param>
/// <param name="guest">The guest who sent the message.</param>
/// <returns>false if muted</returns>
bool Hosting::handleMuting(const char* message, Guest& guest) {

	

	return true;

}

/// <summary>
/// 
/// </summary>
/// <param name="message"></param>
/// <param name="isHidden"></param>
void Hosting::sendHostMessage(const char* message, bool isHidden) {
	static bool isAdmin = true;
	handleMessage(message, _host, true, isHidden);
}

// ============================================================
// 
//  SODA ARCADE
// 
// ============================================================

/// <summary>
/// Creates or updates a room post on the Soda Arcade.
/// </summary>
/// <returns></returns>
bool Hosting::saveRoomOnArcade() {
	
	logMessage("Making a post on Soda Arcade...");
	json j;

	// Parsec stuff
	string peer_id = getSession().hostPeerId;
	string secret = Config::cfg.room.secret;
	string link = (string("https://parsec.gg/g/") + peer_id + "/" + secret + "/").c_str();

	// Is arcade post set?
	if (arcadeRoomID != -1) {
		j["id"] = arcadeRoomID;
	}

	// Properties
	j["link"] = link;
	j["peer_id"] = peer_id;
	j["secret"] = secret;
	j["slug"] = Config::cfg.room.secret;
	
	j["game"] = Config::cfg.room.game;
	j["private"] = Config::cfg.room.privateRoom;
	j["guest_limit"] = Config::cfg.room.guestLimit;
	j["details"] = Config::cfg.room.details;
	//j["latency_limit"] = (Config::cfg.room.latencyLimit ? Config::cfg.room.latencyLimitThreshold : 0);
	j["theme"] = Config::cfg.room.theme;
	//j["artwork_id"] = Config::cfg.room.artworkID;

	string data = j.dump();
	size_t bodySize = sizeof(char) * data.length();

	// Headers
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(bodySize) + "\r\n";
	if (!Config::cfg.arcade.token.empty()) {
		headers += "Authorization: Bearer " + Config::cfg.arcade.token + "\r\n";
	}

	// Make request
	void* response = nullptr;
	size_t responseSize = 0;
	uint16_t status = 0;

	std::string domain = "soda-arcade.test";
	std::string path = "/api/post/smash";
	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, false, "POST", path.c_str(), headers.c_str(),
		data.c_str(), bodySize,
		20000,
		&response, &responseSize, &status
	);
	/*if (responseSize > 0) {
		json result = json::parse(responseStr);
		arcadeRoomID = result["data"]["id"];
		logMessage(to_string(arcadeRoomID));
		logMessage(to_string(status));
	}
	else {
		logMessage("Could not list your room on Soda Arcade for some reason.");
	}*/

	return true;
	
}

/// <summary>
/// Deletes a room post on the Soda Arcade.
/// </summary>
/// <returns></returns>
bool Hosting::deleteRoomOnArcade() {

	if (arcadeRoomID != -1) {
		json j;

		// Properties
		j["post_id"] = arcadeRoomID;
		string data = j.dump();
		size_t bodySize = sizeof(char) * data.length();

		// Headers
		string headers = "Content-Type: application/json\r\n";
		headers += "Content-Length: " + to_string(bodySize) + "\r\n";
		if (!Config::cfg.arcade.token.empty()) {
			headers += "Authorization: Bearer " + Config::cfg.arcade.token + "\r\n";
		}

		// Make request
		void* response = nullptr;
		size_t responseSize = 0;
		uint16_t status = 0;

		std::string domain = "soda-arcade.test";
		std::string path = "/api/post";
		MTY_HttpRequest(
			domain.c_str(), 0, false, "DELETE", path.c_str(), headers.c_str(),
			data.c_str(), bodySize,
			20000,
			&response, &responseSize, &status
		);

	}

	return true;
}

// ============================================================
// 
//  PRIVATE
// 
// ============================================================
void Hosting::initAllModules()
{
	// Instance all gamepads at once
	_connectGamepadsThread = thread([&]() {
		_gamepadClient.sortGamepads();
		_connectGamepadsThread.detach();
	});

	roomStart();
}

void Hosting::submitSilence()
{
	ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequencyHz(), nullptr, 0);
}

void Hosting::liveStreamMedia()
{
	_mediaMutex.lock();
	_isMediaThreadRunning = true;

	static uint32_t sleepTimeMs = 4;
	_mediaClock.setDuration(sleepTimeMs);
	_mediaClock.start();

	while (_isRunning)
	{
		_mediaClock.reset();

		_dx11.captureScreen(_parsec);

		if (Config::cfg.audio.micEnabled && audioIn.isEnabled && audioOut.isEnabled)
		{
			audioIn.captureAudio();
			audioOut.captureAudio();
			if (audioIn.isReady() && audioOut.isReady())
			{
				vector<int16_t> mixBuffer = _audioMix.mix(audioIn.popBuffer(), audioOut.popBuffer());
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequencyHz(), mixBuffer.data(), (uint32_t)mixBuffer.size() / 2);
			}
			else submitSilence();
		}
		else if (audioOut.isEnabled)
		{
			audioOut.captureAudio();
			if (audioOut.isReady())
			{
				vector<int16_t> buffer = audioOut.popBuffer();
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, audioOut.getFrequencyHz(), buffer.data(), (uint32_t)buffer.size() / 2);
			}
			else submitSilence();
		}
		else if (Config::cfg.audio.micEnabled && audioIn.isEnabled)
		{
			audioIn.captureAudio();
			if (audioIn.isReady())
			{
				vector<int16_t> buffer = audioIn.popBuffer();
				ParsecHostSubmitAudio(_parsec, PCM_FORMAT_INT16, (uint32_t)audioIn.getFrequency(), buffer.data(), (uint32_t)buffer.size() / 2);
			}
			else submitSilence();
		}
		else submitSilence();

		sleepTimeMs = _mediaClock.getRemainingTime();
		if (sleepTimeMs > 0)
		{
			Sleep(sleepTimeMs);
		}
	}

	_isMediaThreadRunning = false;
	_mediaMutex.unlock();
	_mediaThread.detach();
}

void Hosting::mainLoopControl()
{
	do
	{
		Sleep(50);
	} while (!_isRunning);

	_isRunning = true;

	_mediaMutex.lock();
	_inputMutex.lock();
	_eventMutex.lock();

	ParsecHostStop(_parsec);
	_isRunning = false;

	_mediaMutex.unlock();
	_inputMutex.unlock();
	_eventMutex.unlock();

	_mainLoopControlThread.detach();
}

void Hosting::pollEvents()
{
	_eventMutex.lock();
	_isEventThreadRunning = true;

	string chatBotReply;

	ParsecGuest* guests = nullptr;
	int guestCount = 0;

	ParsecHostEvent event;

	while (_isRunning)
	{
		if (ParsecHostPollEvents(_parsec, 30, &event)) {
			ParsecGuest parsecGuest = event.guestStateChange.guest;
			ParsecGuestState state = parsecGuest.state;
			Guest guest = Guest(parsecGuest.name, parsecGuest.userID, parsecGuest.id);
			guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
			_guestList.setGuests(guests, guestCount);

			switch (event.type)
			{
			case HOST_EVENT_GUEST_STATE_CHANGE:
				onGuestStateChange(state, guest, event.guestStateChange.status);
				break;

			case HOST_EVENT_USER_DATA:
				char* msg = (char*)ParsecGetBuffer(_parsec, event.userData.key);

				if (event.userData.id == PARSEC_APP_CHAT_MSG)
				{
					handleMessage(msg, guest);
				}

				ParsecFree(_parsec, msg);
				break;
			}
		}
	}

	ParsecFree(_parsec, guests);
	_isEventThreadRunning = false;
	_eventMutex.unlock();
	_eventThread.detach();
}

void Hosting::pollLatency()
{
	_latencyMutex.lock();
	_isLatencyThreadRunning = true;
	int guestCount = 0;
	ParsecGuest* guests = nullptr;
	while (_isRunning)
	{
		Sleep(2000);
		guestCount = ParsecHostGetGuests(_parsec, GUEST_CONNECTED, &guests);
		if (guestCount > 0) {
			_guestList.updateMetrics(guests, guestCount);

			// Latency limiter
			if (Config::cfg.room.latencyLimit) {
				for (size_t mi = 0; mi < guestCount; mi++) {
					MyMetrics m = _guestList.getMetrics(guests[mi].id);

					if (m.averageNetworkLatencySize > 5 &&
						m.averageNetworkLatency > Config::cfg.room.latencyLimitThreshold) {
						ParsecHostKickGuest(_parsec, guests[mi].id);
					}
				}
			}

		}
		
	}
	_isLatencyThreadRunning = false;
	_latencyMutex.unlock();
	_latencyThread.detach();
}

/// <summary>
/// This thread handles all the new Smash Soda features!
/// </summary>
void Hosting::pollSmashSoda() {
	_smashSodaMutex.lock();
	_isSmashSodaThreadRunning = true;
	while (_isRunning) {

		Sleep(200);

		// Handles welcome messages and other new guest stuff
		handleNewGuests();

		// Handles all the automatic button press stuff
		_macro.run();

		// Updated room settings?
		if (Config::cfg.roomChanged) {
			ParsecHostSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
			Config::cfg.roomChanged = false;
		}

	}
	
	_isSmashSodaThreadRunning = false;
	_smashSodaMutex.unlock();
	_smashSodaThread.detach();

}

/// <summary>
/// Checks to see if a guest is a spectator.
/// </summary>
/// <param name="index"></param>
bool Hosting::isSpectator(int index) {

	return MetadataCache::isSpectating(_guestList.getGuests()[index].userID);

}


void Hosting::addNewGuest(Guest guest) {

	NewGuest newGuest;
	newGuest.guest = guest;
	newGuest.timer.setDuration(newGuestList.size() * 2000);
	newGuest.timer.start();
	newGuestList.push_back(newGuest);

	try {
		PlaySound(TEXT("./SFX/new_guest.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
	}
	catch (const std::exception&) {}

}

void Hosting::handleNewGuests() {

	// ParsecHostSendUserData(_parsec, guest.id, HOSTING_CHAT_MSG_ID, test.c_str());

	if (newGuestList.size() > 0) {

		// Get next guest
		NewGuest newGuest = newGuestList.front();

		// Ready to process
		if (newGuestList.front().timer.isFinished()) {

			// Welcome message
			string msg = Config::cfg.chat.welcomeMessage;
			msg = regex_replace(msg, regex("_PLAYER_"), newGuest.guest.name);
			ParsecHostSendUserData(_parsec, newGuestList.front().guest.id, HOSTING_CHAT_MSG_ID, msg.c_str());

			// Remove from new guests
			newGuestList.erase(newGuestList.begin());

		}

	}

}

bool Hosting::isLatencyRunning()
{
	return _isLatencyThreadRunning;
}

bool Hosting::isGamepadRunning() {
	return _isGamepadThreadRunning;
}

void Hosting::pollInputs() {
	
	_inputMutex.lock();
	_isInputThreadRunning = true;

	ParsecGuest inputGuest;
	ParsecMessage inputGuestMsg;

	while (_isRunning)
	{
		if (ParsecHostPollInput(_parsec, 4, &inputGuest, &inputGuestMsg))
		{
			if (!_gamepadClient.lock)
			{
					_gamepadClient.sendMessage(inputGuest, inputGuestMsg);
			}

		}
	}

	_isInputThreadRunning = false;
	_inputMutex.unlock();
	_inputThread.detach();
}

void Hosting::updateButtonLock(LockedGamepadState lockedGamepad) {
	_lockedGamepad = lockedGamepad;
	Config::cfg.input.lockedGamepadLeftTrigger = _lockedGamepad.bLeftTrigger;
	Config::cfg.input.lockedGamepadRightTrigger = _lockedGamepad.bRightTrigger;
	Config::cfg.input.lockedGamepadLX = _lockedGamepad.sThumbLX;
	Config::cfg.input.lockedGamepadLY = _lockedGamepad.sThumbLY;
	Config::cfg.input.lockedGamepadRX = _lockedGamepad.sThumbRX;
	Config::cfg.input.lockedGamepadRY = _lockedGamepad.sThumbRY;
	Config::cfg.input.lockedGamepadButtons = _lockedGamepad.wButtons;
}

bool Hosting::roomStart() {
	
	if (isReady()) {
		ParsecStatus status = ParsecHostStart(_parsec, HOST_DESKTOP, &_hostConfig, _parsecSession.sessionId.c_str());
		Sleep(2000);
		ParsecHostStop(_parsec);
		Sleep(1000);
		status = ParsecHostStart(_parsec, HOST_GAME, &_hostConfig, _parsecSession.sessionId.c_str());

		// Init overlay
		if (Config::cfg.overlay.enabled) {
			
		}

		// Send room to Soda Arcade
		if (!Config::cfg.room.privateRoom) {
			Arcade::instance.createPost();
		}
		
		return status == PARSEC_OK;

	}
	return false;
}

bool Hosting::isFilteredCommand(ACommand* command) {
	
	static vector<COMMAND_TYPE> filteredCommands{ COMMAND_TYPE::IP };

	COMMAND_TYPE type;
	for (vector<COMMAND_TYPE>::iterator it = filteredCommands.begin(); it != filteredCommands.end(); ++it)
	{
		type = command->type();
		if (command->type() == *it)
		{
			return true;
		}
	}

	return false;
}

void Hosting::onGuestStateChange(ParsecGuestState& state, Guest& guest, ParsecStatus& status) {

	static string logMessage;

	/*static string trickDesc = "";
	static Debouncer debouncer = Debouncer(500, [&]() {
		if (_hostConfig.maxGuests > 0 && _guestList.getGuests().size() + 1 == _hostConfig.maxGuests)
		{
			try
			{
				if (trickDesc.size() > 0) trickDesc = "";
				else trickDesc = "-";
				strcpy_s(_hostConfig.desc, trickDesc.c_str());
				applyHostConfig();
			}
			catch (const std::exception&) {}
		}
	});*/

	// Is the connecting guest the host?
	if ((state == GUEST_CONNECTED || state == GUEST_CONNECTING) && (_host.userID == guest.userID))
	{
		_tierList.setTier(guest.userID, Tier::GOD);
		MetadataCache::addActiveGuest(guest);

		addNewGuest(guest);
	}
	else

	if ((state == GUEST_CONNECTED || state == GUEST_CONNECTING) && _banList.isBanned(guest.userID))
	{
		ParsecHostKickGuest(_parsec, guest.id);
		logMessage = _chatBot->formatBannedGuestMessage(guest);
		broadcastChatMessage(logMessage);
		_chatLog.logCommand(logMessage);
	}
	else if ((state == GUEST_CONNECTED || state == GUEST_CONNECTING) && _modList.isModded(guest.userID))
	{
		logMessage = _chatBot->formatModGuestMessage(guest);
		broadcastChatMessage(logMessage);
		_tierList.setTier(guest.userID, Tier::MOD);
		_chatLog.logCommand(logMessage);
		MetadataCache::addActiveGuest(guest);
		addNewGuest(guest);
	}
	else if (state == GUEST_FAILED)
	{
		logMessage = _chatBot->formatGuestConnection(guest, state, status);
		broadcastChatMessage(logMessage);
		_chatLog.logCommand(logMessage);
	}
	else if (state == GUEST_CONNECTED || state == GUEST_DISCONNECTED)
	{
		static string guestMsg;
		guestMsg.clear();
		guestMsg = string(guest.name);

		if (_banList.isBanned(guest.userID)) {
			logMessage = _chatBot->formatBannedGuestMessage(guest);
			broadcastChatMessage(logMessage);
			_chatLog.logCommand(logMessage);
			/*if (_hostConfig.maxGuests > 0 && _guestList.getGuests().size() + 1 == _hostConfig.maxGuests)
				debouncer.start();*/
		}
		else
		{
			logMessage = _chatBot->formatGuestConnection(guest, state, status);
			broadcastChatMessage(logMessage);
			_chatLog.logCommand(logMessage);
		}

		if (state == GUEST_CONNECTED) {
			GuestData data = GuestData(guest.name, guest.userID);
			_guestHistory.add(data);
			MetadataCache::addActiveGuest(guest);

			// Show welcome message
			addNewGuest(guest);

			_overlay.addGuest(to_string(guest.userID), guest.name);

		}
		else {
			
			// Were extra spots made?
			/*if (status != 11 && MetadataCache::preferences.extraSpots > 0) {
				_hostConfig.maxGuests = _hostConfig.maxGuests - 1;
				MetadataCache::preferences.extraSpots--;
				ParsecHostSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
			}*/

			_overlay.removeGuest(to_string(guest.userID));

			// Remove from active guests list
			MetadataCache::removeActiveGuest(guest);

			// Hotseat mode
			if (Config::cfg.hotseat.enabled) {
				Hotseat::instance.pauseUser(guest.userID);
			}

			_guestList.deleteMetrics(guest.id);
			int droppedPads = 0;
			CommandFF command(guest, _gamepadClient, _hotseat);
			command.run();
			if (droppedPads > 0) {
				logMessage = command.replyMessage();
				broadcastChatMessage(logMessage);
				_chatLog.logCommand(logMessage);
			}

			try {
				PlaySound(TEXT("./SFX/guest_leave.wav"), NULL, SND_FILENAME | SND_NODEFAULT | SND_ASYNC);
			}
			catch (const std::exception&) {}
			
		}
	}
}

bool Hosting::removeGame(string name) {

	_gamesList.remove(name, [&](GameData& guest) {
		
	});

	return true;

}

void Hosting::logMessage(string message) {
	string chatbotName = Config::cfg.chatbotName;
	_chatLog.logCommand(chatbotName + " " + message);
	_overlay.sendLogMessage(message);
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
bool Hosting::isHotseatEnabled() {
	return false;
}

/// <summary>
/// Helper function to add fake guests to the guest list.
/// </summary>
/// <param name="count"></param>
void Hosting::addFakeGuests(int count) {

	_guestList.getGuests().reserve(_guestList.getGuests().size() + count);
	for (int i = 0; i < count; ++i) {
		Guest guest = Guest(
			randomString(5),
			i+1, i+1
		);

		_guestList.getGuests().push_back(guest);
		MetadataCache::addActiveGuest(guest);
	}

}

/// <summary>
/// Helper function to generate a random string.
/// </summary>
string Hosting::randomString(const int len) {

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}