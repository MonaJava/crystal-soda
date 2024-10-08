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

/**
* This function is where you check to see if the
* message matches your command prefixes.
**/
ACommand* ChatBotCustom::isCustomCommand(const char* msg, Guest& sender, bool isHost, Tier tier, uint32_t previous) {

	//if (msgIsEqual(msg, YourCommandClass::prefixes()))	return new YourCommandClass(sender, _gamepadClient, _macro);
	if (msgIsEqual(msg, EmptyTime::prefixes()))		return new EmptyTime(sender);
	if (msgStartsWith(msg, TTS::prefixes()))		return new TTS(msg, sender);
	if (msgStartsWith(msg, Queue::prefixes()))		return new Queue(msg, sender, _gamepadClient);
	if (msgStartsWith(msg, ListQueue::prefixes()))	return new ListQueue(msg, sender, _gamepadClient);
	if (msgIsEqual(msg, EmptyQueue::prefixes()))	return new EmptyQueue(msg, sender, _gamepadClient);

	/*
	ADMIN COMMANDS
	The host and moderators can use these commands. Moderators have the
	"ADMIN" tier. The host has the "GOD" tier.
	*/
	if (tier >= Tier::ADMIN || isHost) {
		if (msgStartsWith(msg, EmptyQueue::prefixes()))	return new EmptyQueue(msg, sender, _gamepadClient);
	}

	

	// Returns a default message if no custom command is found, so the bot can still respond.
	return new CommandDefaultMessage(msg, sender, previous, tier, isHost);
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