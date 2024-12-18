#include "ChatBotCustom.h"

/*
* Include all your custom commands here.
* 
* Changes to this file and files inside Commands/Custom
* folder will be ignored by git, to avoid conflicts when
* pulling updates from the repository.
*/

//#include "Commands/Custom/YourCommandClass.h"
#include "Commands/Custom/EmptyTime.h"
#include "Commands/Custom/TTS.h"
#include "Commands/Custom/Queue.h"
#include "Commands/Custom/ListQueue.h"
#include "Commands/Custom/EmptyQueue.h"
#include "Commands/Custom/ExitQueue.h"
#include "Commands/Custom/Pleb.h"
#include "Commands/Custom/DioHelp.h"
#include "Commands/Custom/DioVersion.h"
#include "Commands/Custom/SetRole.h"
#include "Commands/Custom/CommandPlayTime.h"
#include "Commands/Custom/KickRandom.h"
#include "Commands/Custom/VoteCommands.h"
#include "Commands/Custom/VoteKick.h"
#include "Commands/Custom/VoteQuestion.h"
#include "Commands/Custom/Ignore.h"

/**
* This function is where you check to see if the
* message matches your command prefixes.
**/
ACommand* ChatBotCustom::isCustomCommand(const char* msg, Guest& sender, bool isHost, Tier tier, uint32_t previous) {
	/*if (isCommand(msg, YourCommandClass::prefixes())) {
	return new YourCommandClass(msg, sender, _parsec, _guests, _guestHistory);
}*/

	string role = "guest";
	switch (tier)
	{
		case Tier::NOOB:
			role = "noob";
		case Tier::PLEB:
			role = "guest";
		case Tier::MOD:
			role = "mod";
		case Tier::GOD:
		case Tier::ADMIN:
			role = "host";
		default:
			role = "guest";
	}

	Role r = GuestRoles::instance.getRole(sender.userID);
	role = r.key;


	// Split the message by spaces
	vector<string> permissions = split(Config::cfg.permissions.role[role].permissions, ' ');
	vector<string> tokens = split(msg, ' ');
	bool allowedCommand = false;

	// Does the first token match any of the patterns?
	vector<string>::iterator pi = permissions.begin();
	for (; pi != permissions.end(); ++pi) {
		if (tokens[0] == *pi) {
			allowedCommand = true;
		}
	}
	if (!allowedCommand and !isHost and permissions[0] != "<ALLCOMMANDS>")
	{
		return new CommandDefaultMessage(msg, sender, previous, tier, isHost);
	}
	if (isCommand(msg, CommandPlayTime::prefixes()))	return new CommandPlayTime(msg, sender, _guests);
	
	if (isCommand(msg, EmptyTime::prefixes()))			return new EmptyTime(msg, sender);
	if (isCommand(msg, Queue::prefixes()))				return new Queue(msg, sender, _gamepadClient);
	if (isCommand(msg, ListQueue::prefixes()))			return new ListQueue(msg, sender, _gamepadClient);
	if (isCommand(msg, ExitQueue::prefixes()))			return new ExitQueue(msg, sender, _gamepadClient);
	if (isCommand(msg, DioHelp::prefixes()))			return new DioHelp(msg, sender);
	if (isCommand(msg, DioVersion::prefixes()))			return new DioVersion(msg, sender);

	if (isCommand(msg, VoteQuestion::prefixes()))		return new VoteQuestion(msg, sender);
	if (isCommand(msg, VoteKick::prefixes()))			return new VoteKick(msg, sender, _parsec, _guests, isHost);
	if (isCommand(msg, YayVote::prefixes()))			return new YayVote(msg, sender);
	if (isCommand(msg, NayVote::prefixes()))			return new NayVote(msg, sender);
	if (isCommand(msg, Ignore::prefixes()))				return new Ignore(msg, sender, _guests, _host, _guests);

	/*
	ADMIN COMMANDS
	The host and moderators can use these commands. Moderators have the
	"ADMIN" tier. The host has the "GOD" tier.
	*/
	
	//if (tier >= Tier::ADMIN || isHost) {
		if (isCommand(msg, KickRandom::prefixes()))		return new KickRandom(msg, sender, _parsec, _guests, isHost);
		if (isCommand(msg, EmptyQueue::prefixes()))		return new EmptyQueue(msg, sender, _gamepadClient);
		if (isCommand(msg, Pleb::prefixes()))			return new Pleb(msg, sender, _parsec, _guests, _guestHistory, _gamepadClient);
		if (isCommand(msg, ClearVote::prefixes()))		return new ClearVote(msg, sender);

	//}

	/*
	HOST COMMANDS
	Only the host can use these commands. The host has the "GOD" tier.
	*/
	
	//if (tier >= Tier::ADMIN || isHost) {

		map<string, Role>::iterator it;

		for (it = Roles::r.list.begin(); it != Roles::r.list.end(); it++)
		{
			vector<const char*> roleprefix = { it->second.commandPrefix.c_str() };
			int len = it->second.commandPrefix.length();
			if (!roleprefix.empty() && len >= 4)
			{
				if (isCommand(msg, roleprefix)) {
					return new SetRole(msg, sender, _parsec, _guests, _guestHistory, it->second);
				}
			}
		}
	//}


	/*
	PERMISSION COMMANDS
	*/
	if (Config::cfg.permissions.role[role].useSFX)
	{
		if (msgStartsWith(msg, TTS::prefixes()))		return new TTS(msg, sender);
	}

	// Returns a default message if no custom command is found, so the bot can still respond.
	return new CommandDefaultMessage(msg, sender, previous, tier, isHost);
}

/*
Add all the help/descriptions for the commands.
*/
void ChatBotCustom::addHelp() {

	//addCmdHelp("!yourcommand", "Description", Tier::GUEST);
	addCmdHelp("/help", "Lists Dio's commands", Tier::GUEST);
	addCmdHelp("/version", "I'll change this every time theres a significant update", Tier::GUEST);
	addCmdHelp("/poll", "Begins a vote on a provided question", Tier::GUEST);
	addCmdHelp("/votekick", "Begins a vote to kick a specified user", Tier::GUEST);
	addCmdHelp("/yay", "Agrees to a vote", Tier::GUEST);
	addCmdHelp("/nay", "Disagrees with a vote", Tier::GUEST);
	addCmdHelp("/startcooldown", "Removes all your hotseat playtime", Tier::GUEST);
	addCmdHelp("/queue", "Reserves a gamepad for you to use when the owner drops", Tier::GUEST);
	addCmdHelp("/listqueue", "Shows who's in a queue", Tier::GUEST);
	addCmdHelp("/exitqueue", "Exits the queue", Tier::GUEST);
	addCmdHelp("/tts", "Text to speech", Tier::GUEST);
	addCmdHelp("/emptyqueue", "Removes all users from the queue of a gamepad", Tier::MOD);
	addCmdHelp("/randkick", "Kicks a random guest", Tier::MOD);
	addCmdHelp("/voteclear", "Stops a vote from finishing", Tier::MOD);
	map<string, Role>::iterator it;

	for (it = Roles::r.list.begin(); it != Roles::r.list.end(); it++)
	{
		string roleprefix =  it->second.commandPrefix;
		int len = roleprefix.length();
		if (len >= 4)
		{
			addCmdHelp(roleprefix, "Sets a user's role to " + it->second.name, Tier::GOD);
		}
	}
}

/**
* Don't edit anything below this line. Use these functions
* to check if a message starts with a pattern or is equal to
* a pattern.
**/

bool ChatBotCustom::msgStartsWith(const char* msg, const char* pattern) {
	return Stringer::startsWithPattern(msg, pattern);
}

bool ChatBotCustom::msgStartsWith(const char* msg, vector<const char*> patterns) {
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

bool ChatBotCustom::msgIsEqual(const char* msg, const char* pattern) {
	return (strcmp(msg, pattern) == 0);
}

bool ChatBotCustom::msgIsEqual(const char* msg, vector<const char*> patterns) {
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

std::vector<std::string> ChatBotCustom::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}


bool ChatBotCustom::isCommand(const char* msg, vector<const char*> patterns) {
	// Split the message by spaces
	vector<string> tokens = split(msg, ' ');

	// Does the first token match any of the patterns?
	vector<const char*>::iterator pi = patterns.begin();
	for (; pi != patterns.end(); ++pi) {
		if (tokens[0] == *pi) {
			return true;
		}
	}

	return false;
}

// Get the arguments of a command
vector<string> ChatBotCustom::getArguments(const char* msg) {
	vector<string> tokens = split(msg, ' ');
	tokens.erase(tokens.begin());
	return tokens;
}

/*
Add a command's help/description.
*/
void ChatBotCustom::addCmdHelp(std::string command, std::string desc, Tier tier) {
	commands.push_back(CommandInfo(command, desc, tier));
}