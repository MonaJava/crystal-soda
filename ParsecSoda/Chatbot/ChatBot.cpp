#include "ChatBot.h"


ACommand * ChatBot::identifyUserDataMessage(const char* msg, Guest &sender, bool isHost) {
	const uint32_t previous = this->_lastUserId;
	setLastUserId(BOT_GUESTID);

	// Get the user's tier and permissions
	Tier tier = Cache::cache.tierList.getTier(sender.userID);

	// Is this a Soda Cop, laying down the law?
	if (!isHost && Cache::cache.isSodaCop(sender.userID)) {
		if (msgStartsWith(msg, CommandKick::prefixes()))		return new CommandKick(msg, sender, _parsec, _guests, isHost);
		if (msgStartsWith(msg, CommandBan::prefixes()))			return new CommandBan(msg, sender, _parsec, _guests, _guestHistory);
		if (msgIsEqual(msg, CommandVersion::prefixes()))		return new CommandVersion(sender);
	}

	// Is this a custom command?
	ACommand* custom = _chatBotCustom->isCustomCommand(msg, sender, isHost, tier, previous);
	if (custom->isBotCommand) {
		return custom;
	}

	/*
	REGULAR USER COMMANDS
	Any user can use these commands. Regular users have the "PLEB" tier
	(hey I didn't name people that, blame Fl�vio!)
	*/
	if (msgStartsWith(msg, CommandBB::prefixes()))			return new CommandBB(_gamepadClient, _macro, sender);
	if (msgStartsWith(msg, CommandBonk::prefixes()))		return new CommandBonk(msg, sender, _guests, _host);
	if (msgIsEqual(msg, CommandDiscord::prefixes()))		return new CommandDiscord(sender);
	if (msgIsEqual(msg, CommandFF::prefixes()))				return new CommandFF(sender, _gamepadClient, _hotseat);
	if (msgIsEqual(msg, Command8Ball::prefixes()))			return new Command8Ball(sender);
	if (msgIsEqual(msg, CommandFortune::prefixes()))		return new CommandFortune(sender);
	if (msgIsEqual(msg, CommandHelp::prefixes()))			return new CommandHelp(sender);
	if (msgStartsWith(msg, CommandKeyboard::prefixes()))	return new CommandKeyboard(msg, sender, _gamepadClient);
	if (msgIsEqual(msg, CommandMirror::prefixes()))			return new CommandMirror(sender, _gamepadClient);
	if (msgIsEqual(msg, CommandOne::prefixes()))			return new CommandOne(sender, _gamepadClient);
	if (msgIsEqual(msg, CommandPads::prefixes()))			return new CommandPads(_gamepadClient);
	if (msgStartsWith(msg, CommandSpectate::prefixes()))	return new CommandSpectate(msg, sender, _guests, _gamepadClient, _hotseat);
	if (msgStartsWith(msg, CommandSwap::prefixes()))		return new CommandSwap(msg, sender, _gamepadClient);
	if (msgIsEqual(msg, CommandTriangle::prefixes()))		return new CommandTriangle(sender, _gamepadClient, _macro);
	if (msgStartsWith(msg, CommandPing::prefixes()))		return new CommandPing(msg, sender, _guests, _host);
	if (msgStartsWith(msg, CommandPlayTime::prefixes()))	return new CommandPlayTime(msg, sender, _guests, _host);
	if (msgIsEqual(msg, CommandRPG::prefixes()))			return new CommandRPG(msg, sender, _guests);
	if (msgIsEqual(msg, CommandRollCall::prefixes()))		return new CommandRollCall(msg, sender, _guests);
	if (msgStartsWith(msg, CommandSFX::prefixes()))			return new CommandSFX(msg, sender);
	if (msgIsEqual(msg, CommandStopSFX::prefixes()))		return new CommandStopSFX(sender);

	/*
	ADMIN COMMANDS
	The host and moderators can use these commands. Moderators have the 
	"ADMIN" tier. The host has the "GOD" tier.
	*/
	if (tier >= Tier::ADMIN || isHost) {

		if (msgStartsWith(msg, CommandBan::prefixes()))			return new CommandBan(msg, sender, _parsec, _guests, _guestHistory);
		if (msgStartsWith(msg, CommandHotseat::prefixes()))		return new CommandHotseat(sender, _hotseat);
		if (msgIsEqual(msg, CommandDC::prefixes()))			return new CommandDC(msg, _gamepadClient);
		if (msgStartsWith(msg, CommandDecrease::prefixes()))	return new CommandDecrease(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandDecreaseCD::prefixes()))	return new CommandDecreaseCD(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandExtend::prefixes()))		return new CommandExtend(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandExtendCD::prefixes()))	return new CommandExtendCD(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandKick::prefixes()))		return new CommandKick(msg, sender, _parsec, _guests, isHost);
		if (msgStartsWith(msg, CommandLimit::prefixes()))		return new CommandLimit(msg, _guests, _gamepadClient);
		//if (msgIsEqual(msg, CommandLock::prefixes()))			return new CommandLock(msg, sender, _gamepadClient);
		if (msgStartsWith(msg, CommandLockAll::prefixes()))		return new CommandLockAll(_gamepadClient);
		if (msgStartsWith(msg, CommandMute::prefixes()))		return new CommandMute(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandRC::prefixes()))			return new CommandRC(msg, _gamepadClient);
		if (msgStartsWith(msg, CommandRestart::prefixes()))		return new CommandRestart();
		//if (msgStartsWith(msg, CommandSpot::prefixes()))		return new CommandSpot(msg, _hostConfig);
		if (msgStartsWith(msg, CommandStrip::prefixes()))		return new CommandStrip(msg, sender, _gamepadClient, _hotseat);
		if (msgStartsWith(msg, CommandStripAll::prefixes()))	return new CommandStripAll(msg, _gamepadClient);
		if (msgIsEqual(msg, CommandUnban::prefixes()))		return new CommandUnban(msg, sender, _guestHistory);
		if (msgIsEqual(msg, CommandUnbanLastIP::prefixes()))	return new CommandUnbanLastIP(msg);
		if (msgStartsWith(msg, CommandUnmute::prefixes()))		return new CommandUnmute(msg, sender, _guests, _host);
		if (msgStartsWith(msg, CommandVerify::prefixes()))		return new CommandVerify(msg, sender, _guests, _guestHistory);
		if (msgIsEqual(msg, CommandDCAll::prefixes()))			return new CommandDCAll(_gamepadClient);

	}

	/*
	HOST COMMANDS
	Only the host can use these commands. The host has the "GOD" tier.
	*/
	if (tier >= Tier::GOD || isHost) {
		if (msgStartsWith(msg, CommandAddXbox::prefixes()))		return new CommandAddXbox(_gamepadClient);
		if (msgStartsWith(msg, CommandAddPS::prefixes()))		return new CommandAddPS(_gamepadClient);
		if (msgStartsWith(msg, CommandGuests::prefixes()))		return new CommandGuests(msg, _hostConfig);
		if (msgStartsWith(msg, CommandName::prefixes()))		return new CommandName(msg, _hostConfig);
		if (msgIsEqual(msg, CommandPrivate::prefixes()))		return new CommandPrivate(_hostConfig);
		if (msgIsEqual(msg, CommandPublic::prefixes()))			return new CommandPublic(_hostConfig);
		if (msgIsEqual(msg, CommandSetConfig::prefixes()))		return new CommandSetConfig(_parsec, &_hostConfig, _parsecSession.sessionId.c_str());
		if (msgStartsWith(msg, CommandSpeakers::prefixes()))	return new CommandSpeakers(msg, _audioOut);
		if (msgIsEqual(msg, CommandQuit::prefixes()))			return new CommandQuit(_hostingLoopController);
		if (msgStartsWith(msg, CommandMod::prefixes()))			return new CommandMod(msg, sender, _parsec, _guests, _guestHistory);
		if (msgStartsWith(msg, CommandUnmod::prefixes()))		return new CommandUnmod(msg, sender, _guestHistory);
		if (msgStartsWith(msg, CommandUnVIP::prefixes()))		return new CommandUnVIP(msg, sender, _guestHistory);
		if (msgStartsWith(msg, CommandVIP::prefixes()))			return new CommandVIP(msg, sender, _parsec, _guests, _guestHistory);
	}

	this->setLastUserId(previous);
	return new CommandDefaultMessage(msg, sender, _lastUserId, tier, isHost);
}

const uint32_t ChatBot::getLastUserId() const
{
	return this->_lastUserId;
}

const std::string ChatBot::formatGuestConnection(Guest guest, ParsecGuestState state, ParsecStatus status)
{
	setLastUserId(BOT_GUESTID);

	std::ostringstream reply;
	if (state == GUEST_CONNECTED)
	{
		reply << "@join \t\t " << guest.name << " #" << guest.userID << "\0";
	}
	else if (state == GUEST_FAILED)
	{
		reply << "!" << status << " \t\t " << guest.name << " #" << guest.userID << "\0";
	}
	else
	{
		switch (status)
		{
		case 5:
			reply << "!kick \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		case -12007:
			reply << "!timeout \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		case -13014:
			reply << "!quit \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		default:
			reply << "!" << status << " \t\t " << guest.name << " #" << guest.userID << "\0";
			break;
		}
	}

	const std::string formattedMessage = reply.str();
	reply.clear();
	reply.flush();
	return formattedMessage;
}

const std::string ChatBot::formatBannedGuestMessage(Guest guest)
{
	std::ostringstream reply;
	reply << "!ban \t\t " << guest.name << " #" << guest.userID << "\0";

	return reply.str();
}

const std::string ChatBot::formatModGuestMessage(Guest guest) {

	std::ostringstream reply;
	reply << Config::cfg.chatbotName + "Moderator \n\t\t" << guest.name << " \t (#" << guest.userID << ") has joined!\0";

	return reply.str();

}

CommandBotMessage ChatBot::sendBotMessage(const char* msg)
{
	CommandBotMessage message(msg);
	message.run();
	setLastUserId(BOT_GUESTID);
	return message;
}

bool ChatBot::msgStartsWith(const char* msg, const char* pattern)
{
	return Stringer::startsWithPattern(msg, pattern);
}

bool ChatBot::msgStartsWith(const char* msg, vector<const char*> patterns)
{
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if (Stringer::startsWithPattern(msg, *pi))
		{
			return true;
		}
	}
	return false;
}

bool ChatBot::msgIsEqual(const char* msg, const char* pattern)
{
	return (strcmp(msg, pattern) == 0);
}

bool ChatBot::msgIsEqual(const char* msg, vector<const char*> patterns)
{
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi)
	{
		if ((strcmp(msg, *pi) == 0))
		{
			return true;
		}
	}
	return false;
}

void ChatBot::setLastUserId(uint32_t lastId)
{
	this->_lastUserId = lastId;
}

void ChatBot::updateSettings()
{
	//_basicVersion = Config::cfg.general.basicVersion;
}