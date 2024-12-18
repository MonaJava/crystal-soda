#pragma once

#include "../../ACommand.h"

class VoteData
{
public:
	ACommand* _votingCommand;
	string _subject = "";
	bool _isVoting = false;
	int _yayVotes = 0;
	int _nayVotes = 0;
	vector<int> _votingUsers;

	void setCommand(ACommand* command)
	{
		_votingCommand = command;
		
		if (command->getArgs().size() > 0)
		{
			_subject = command->getArgString();
		}
		_isVoting = true;
		_yayVotes = 0;
		_nayVotes = 0;
		_votingUsers.clear();
	}
	void endVoting()
	{
		_votingCommand = nullptr;
		_subject = "";
		_isVoting = false;
		_yayVotes = 0;
		_nayVotes = 0;
		_votingUsers.clear();
	}
	static VoteData instance;
};

class YayVote : public ACommand
{
public:

	/**
	 * @brief Construct a new YayVote object
	 *
	 * @param sender
	 */
	YayVote(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		
		if (VoteData::instance._isVoting == true)
		{
			vector<int>::iterator i;
			for (i = VoteData::instance._votingUsers.begin(); i != VoteData::instance._votingUsers.end(); ++i) {
				if (*i == _sender.userID) {
					setReply(_sender.name + ", you have already voted!");
					return false;
				}
			}
			VoteData::instance._votingUsers.push_back(_sender.userID);
			VoteData::instance._yayVotes++;
			setReply(_sender.name + " has voted yes! Total supporting votes: " + std::to_string(VoteData::instance._yayVotes) + "\0");
		}
		else
		{
			setReply("No vote undergoing");
			return false;
		}
		int voteDifference = VoteData::instance._yayVotes - VoteData::instance._nayVotes;
		int checkNum = Config::cfg.room.guestLimit / 2;

		if (voteDifference > checkNum)
		{
			VoteData::instance._votingCommand->run();
			setReply("A majority has been reached! Yes: " + std::to_string(VoteData::instance._yayVotes)
				+ ", No:" + std::to_string(VoteData::instance._nayVotes) + "\n" + VoteData::instance._votingCommand->getReply());
			VoteData::instance.endVoting();
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/yay" };
	}

protected:
	Guest& _sender;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/yay " };
	}
};

class NayVote : public ACommand
{
public:

	/**
	 * @brief Construct a new YayVote object
	 *
	 * @param sender
	 */
	NayVote(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		if (VoteData::instance._isVoting == true)
		{
			vector<int>::iterator i;
			for (i = VoteData::instance._votingUsers.begin(); i != VoteData::instance._votingUsers.end(); ++i) {
				if (*i == _sender.userID) {
					setReply(_sender.name + ", you have already voted!");
					return false;
				}
			}
			VoteData::instance._votingUsers.push_back(_sender.userID);
			VoteData::instance._nayVotes++;
			setReply(_sender.name + " has voted no! Total opposing votes: " + std::to_string(VoteData::instance._nayVotes) + "\0");
		}
		else
		{
			setReply("No vote undergoing");
		}

		int voteDifference = VoteData::instance._yayVotes - VoteData::instance._nayVotes;
		int checkNum = Config::cfg.room.guestLimit / 2;

		if (voteDifference < -checkNum)
		{
			VoteData::instance._votingCommand->run();
			setReply("A majority has been reached! Yes: " + std::to_string(VoteData::instance._yayVotes)
				+ ", No:" + std::to_string(VoteData::instance._nayVotes) + "\n" + VoteData::instance._votingCommand->getReply());
			VoteData::instance.endVoting();
		}

		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/nay" };
	}

protected:
	Guest& _sender;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/nay " };
	}
};

class ClearVote : public ACommand
{
public:

	/**
	 * @brief Construct a new YayVote object
	 *
	 * @param sender
	 */
	ClearVote(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {
		if (VoteData::instance._isVoting == true)
		{
			VoteData::instance.endVoting();
		}
		else
		{
			setReply("No vote undergoing");
		}
		return true;
	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/voteclear" };
	}

protected:
	Guest& _sender;

	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/voteclear " };
	}
};