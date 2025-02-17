#pragma once

#include <vector>
#include <thread>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <chrono>
#include <ctime>
#include "../Helpers/Stopwatch.h"
#include "../Models/GuestData.h"

/// <summary>
/// Handles the hotseat system.
/// </summary>
class DioStuff {
public:
	

	DioStuff();

	void Start();
	void Stop();

	Stopwatch* stopwatch = new Stopwatch();

	static DioStuff instance;

private:
	thread _thread;
	bool isRunning = false;

};