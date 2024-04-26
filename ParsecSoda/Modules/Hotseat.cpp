#include "../Hosting.h"
extern Hosting g_hosting;
#include "Hotseat.h"

Hotseat Hotseat::instance = Hotseat::Hotseat();

/// <summary>
/// Constructor
/// </summary>
Hotseat::Hotseat() {
	
}

/// <summary>
/// Starts the hotseat thread.
/// </summary>
void Hotseat::Start() {

	// Start the hotseat thread
	if (!running) {
		running = true;
		hotseatThread = std::thread([&] {

			while (running) {

				// Get current timestamp
				std::time_t currentTime = getCurrentTimestamp();

				// Loop through all users
				for (HotseatUser& user : users) {

					// A user can try to play again
					user.cooldown = false;

					// Check if user is in seat
					if (user.inSeat) {

						// Check if user has time remaining
						if (user.stopwatch->isFinished()) {

							// Stop the stopwatch
							user.stopwatch->stop();

							// Remove user from seat
							user.inSeat = false;

							// Strip pads
							g_hosting.getGamepadClient().strip(user.userId);

							// Log user removed
							Log("User " + user.userName + " has been removed from the hotseat. They must wait " + to_string(Config::cfg.hotseat.resetTime) + " minutes.");

						} else {

							// Log user time remaining
							Log("User " + user.userName + " has " + user.stopwatch->getRemainingTime() + " remaining.");

						}

					}

				}

				// Sleep for a minute
				std::this_thread::sleep_for(std::chrono::minutes(1));
			}
		});
		hotseatThread.detach();
	}

}

/// <summary>
/// Stops the hotseat thread.
/// </summary>
void Hotseat::Stop() {

	// Stop the hotseat thread
	running = false;

	// Stop all active users
	for (HotseatUser& user : users) {
		user.inSeat = false;
		user.stopwatch->stop();
	}

}

/// <summary>
/// Find a user by id.
/// </summary>
Hotseat::HotseatUser* Hotseat::getUser(int id) {

	// Find user
	for (HotseatUser& user : users) {
		if (user.userId == id) {
			return &user;
		}
	}

	// If user not found, return nullptr
	return nullptr;

}

/// <summary>
/// Check if a user can be seated.
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
bool Hotseat::checkUser(int id, string name) {

	// Get current timestamp
	std::time_t currentTime = getCurrentTimestamp();

	// Find user
	HotseatUser* user = getUser(id);
	if (user == nullptr) {

		// Create a new user
		_createUser(id, name);

		return true;

	}
	else {

		// Check if user has time remaining
		if (user->stopwatch->getRemainingMs() > 0) {

			// User still has time remaining, so they can join
			// Log user reseated
			Log("User " + user->userName + " has been reseated. They have " + user->stopwatch->getRemainingTime() + " remaining.");

			// Start the stopwatch
			user->stopwatch->resume();

			// Add user to seat
			user->inSeat = true;

			return true;
		}
		else {
			// User has no time remaining

			// Get time since last played
			int minutesSinceLastPlayed = getMinutesDifference(user->timeLastPlayed, currentTime);

			// Check if user is on cooldown
			if (minutesSinceLastPlayed < Config::cfg.hotseat.resetTime) {
				// User is on cooldown, prevent reseating
				// Log user on cooldown
				if (!user->cooldown) {
					Log("User " + user->userName + " is on cooldown. They must wait " + to_string(Config::cfg.hotseat.resetTime - minutesSinceLastPlayed) + " minutes.");
					// Prevent spamming
					user->cooldown = true;
				}

				return false;

			}
			else {

				if (user->stopwatch->isPaused()) {

					// User is not on cooldown, reseat them
					// Log user reseated
					Log("User " + user->userName + " is now in the seat. They have " + user->stopwatch->getRemainingTime() + " left.");

					// Start the stopwatch
					user->stopwatch->resume();

				} else {

					// User is not on cooldown, reseat them
					// Log user reseated
					Log("User " + user->userName + " has been reseated. They have " + to_string(Config::cfg.hotseat.playTime) + " minutes.");

					// Set the stopwatch time
					user->stopwatch->start(Config::cfg.hotseat.playTime);

				}

				// Add user to seat
				user->inSeat = true;

				// Update last played time
				user->timeLastPlayed = currentTime;

				return true;
			}
		}
	}

	return true;
}

/// <summary>
/// Creates a new Hotseat user.
/// </summary>
/// <param name="id"></param>
/// <param name="name"></param>
void Hotseat::_createUser(int id, std::string name) {
    // Create a new user
    HotseatUser user;
    user.userId = id;
    user.userName = name;
    user.inSeat = true;
    user.timeLastPlayed = getCurrentTimestamp();
    user.stopwatch = new StopwatchTimer();

	// Set the stopwatch time
	user.stopwatch->start(Config::cfg.hotseat.playTime);

    // Add the user to the list
    users.push_back(user);

    // Log user added
    Log("User " + name + " added to hotseat. They have " + std::to_string(Config::cfg.hotseat.playTime) + " minutes.");
}

/// <summary>
/// Remove a user from hotseat.
/// </summary>
void Hotseat::pauseUser(int id) {

	// Get current timestamp
	std::time_t currentTime = getCurrentTimestamp();

	// Find user
	HotseatUser* user = getUser(id);
	if (user != nullptr) {

		// Was user in seat?
		if (user->inSeat) {

			// Stop the stopwatch
			user->stopwatch->pause();

			// Remove user from seat
			user->inSeat = false;

			// Record time last played
			user->timeLastPlayed = currentTime;

			// Did the user have time remaining?
			if (user->stopwatch->getRemainingMs() > 0) {

				// Log user paused
				Log("User " + user->userName + " left the seat with " + user->stopwatch->getRemainingTime() + " remaining.");

			}
		
		}

	}

}

/// <summary>
/// Extend a user's hotseat time.
/// </summary>
void Hotseat::extendUser(int id, long minutes) {

	// Find user
	HotseatUser* user = getUser(id);
	if (user != nullptr) {

		// Extend the user's time in minutes
		user->stopwatch->addMinutes(minutes);

		// Log user extended
		Log("User " + user->userName + " has been extended by " + to_string(minutes) + " minutes.");

	}

}

/// <summary>
/// Deduct time from a user.
/// </summary>
void Hotseat::deductUser(int id, long minutes) {

	// Find user
	HotseatUser* user = getUser(id);
	if (user != nullptr) {

		// Deduct the user's time in minutes
		user->stopwatch->subtractMinutes(minutes);

		// Log user deducted
		Log("User " + user->userName + " has been deducted by " + to_string(minutes) + " minutes.");

	}

}

/// <summary>
/// Get the time remaining for a user.
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
string Hotseat::getUserTimeRemaining(int id) {

	// Find user
	HotseatUser* user = getUser(id);
	if (user != nullptr) {

		// Get the user's time remaining
		return user->stopwatch->getRemainingTime();

	}

	return "";

}

/// <summary>
/// Log a message and print in chat.
/// </summary>
/// <param name="message"></param>
void Hotseat::Log(string message) {
	
	// Format message
	string msg = "[HOTSEAT] ";
	msg += message;

	// Print message
	g_hosting.logMessage(msg);
	g_hosting.broadcastChatMessage(msg);

}

/// <summary>
/// Helper function to get the current timestamp.
/// </summary>
/// <returns></returns>
std::time_t Hotseat::getCurrentTimestamp() {

	// Get the current time point
	auto currentTime = std::chrono::system_clock::now();

	// Convert the time point to a duration since the epoch
	auto durationSinceEpoch = currentTime.time_since_epoch();

	// Convert the duration to seconds
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);

	// Return the timestamp as time_t type
	return seconds.count();

}

/// <summary>
/// Helper function to get the minutes difference between two timestamps.
/// </summary>
/// <param name="timestamp1"></param>
/// <param name="timestamp2"></param>
/// <returns></returns>
int Hotseat::getMinutesDifference(std::time_t timestamp1, std::time_t timestamp2) {

	// Calculate the difference in seconds
	std::time_t difference = timestamp2 - timestamp1;

	// Convert the difference to minutes
	int minutesDifference = difference / 60;

	return minutesDifference;
}

/// <summary>
/// Helper function to add minutes to a timestamp.
/// </summary>
/// <param name="timestamp"></param>
/// <param name="minutesToAdd"></param>
/// <returns></returns>
std::time_t Hotseat::addMinutesToTimestamp(std::time_t timestamp, int minutesToAdd) {

	// Convert the minutes to seconds
	std::time_t secondsToAdd = minutesToAdd * 60;

	// Add the seconds to the timestamp
	std::time_t newTimestamp = timestamp + secondsToAdd;

	return newTimestamp;
}

/// <summary>
/// Helper function to subtract minutes from a timestamp.
/// </summary>
/// <param name="timestamp"></param>
/// <param name="minutesToSubtract"></param>
/// <returns></returns>
std::time_t Hotseat::subtractMinutesFromTimestamp(std::time_t timestamp, int minutesToSubtract) {

	// Convert the minutes to seconds
	std::time_t secondsToSubtract = minutesToSubtract * 60;

	// Subtract the seconds from the timestamp
	std::time_t newTimestamp = timestamp - secondsToSubtract;

	return newTimestamp;

}

/// <summary>
/// Get the remaining cooldown time for a user.
/// </summary>
/// <param name="id"></param>
/// <returns></returns>
int Hotseat::getCoolDownTime(int id) {
	HotseatUser* user = getUser(id);
	if (user != nullptr) {

		// If the user still has time remaining
		if (user->stopwatch->getRemainingMs() > 0) {
			return 0;
		}

		std::time_t currentTime = getCurrentTimestamp();
		int minutesSinceLastPlayed = getMinutesDifference(user->timeLastPlayed, currentTime);
		if (minutesSinceLastPlayed < Config::cfg.hotseat.resetTime) {
			return Config::cfg.hotseat.resetTime - minutesSinceLastPlayed;
		}
	}
	return 0;
}