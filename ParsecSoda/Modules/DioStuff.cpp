#include "../Hosting.h"
extern Hosting g_hosting;
#include "DioStuff.h"

DioStuff DioStuff::instance = DioStuff::DioStuff();

/// <summary>
/// Constructor
/// </summary>
DioStuff::DioStuff() {

}

/// <summary>
/// Start the DioStuff thread
/// </summary>
void DioStuff::Start() {

	// If not already running
	if (isRunning) {
		return;
	}

	// Start the thread lambda
	isRunning = true;
	thread([this]() {

		// Loop
		while (isRunning) {





			// Sleep
			this_thread::sleep_for(chrono::seconds(2));

		}

		}).detach();

}

/// <summary>
/// Stop the DioStuff thread
/// </summary>
void DioStuff::Stop() {

	isRunning = false;

}
