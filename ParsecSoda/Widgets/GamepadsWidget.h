#pragma once

#include "../imgui/imgui.h"
#include "../Hosting.h"
#include "../globals/AppIcons.h"
#include "../globals/AppStyle.h"
#include "../Helpers/Stopwatch.h"
#include "ToggleIconButtonWidget.h"
#include "TitleTooltipWidget.h"
#include "AnimatedGamepadWidget.h"
#include "LoadingRingWidget.h"
#include "IntRangeWidget.h"
#include "LoadingRingWidget.h"


class GamepadsWidget
{
public:
	GamepadsWidget(Hosting& hosting);
	bool render(bool& showWindow);

private:
	// Dependency injection
	Hosting& _hosting;
	
	// Attributes
	string _logBuffer;
	vector<AGamepad*>& _gamepads;

	void renderPadInputTypeIcon(AGamepad* pad, const size_t& gamepadIndex, bool& refreshGamepads);
};