#include "AppColors.h"

ImVec4 AppColors::title;
ImVec4 AppColors::label;
ImVec4 AppColors::input;
ImVec4 AppColors::alert;
ImVec4 AppColors::error;

ImVec4 AppColors::white;
ImVec4 AppColors::black;
ImVec4 AppColors::invisible;

ImVec4 AppColors::primary;
ImVec4 AppColors::secondary;

void AppColors::init()
{
	title = ImVec4(0.05f, 0.03f, 0.07f, 1.0f);
	label = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	input = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
	alert = ImVec4(0.00f, 0.67f, 0.41f, 1.00f);
	error = ImVec4(0.75f, 0.16f, 0.28f, 1.00f);

	white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	black = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	invisible = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);

	primary = ImVec4(0.00f, 0.47f, 0.80f, 1.00f);
	secondary = ImVec4(0.02f, 0.29f, 0.53f, 1.00f);
}

ImVec4 AppColors::alpha(ImVec4& color, float alpha)
{
	return ImVec4(color.x, color.y, color.z, alpha);
}

void AppColors::pushColor(ImVec4& color)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color);
}

void AppColors::pop()
{
	ImGui::PopStyleColor();
}

void AppColors::pushPrimary()
{
	pushColor(primary);
}

void AppColors::pushSecondary()
{
	pushColor(secondary);
}

void AppColors::pushTitle()
{
	pushColor(title);
}

void AppColors::pushLabel()
{
	pushColor(label);
}

void AppColors::pushInput()
{
	pushColor(input);
}

void AppColors::pushAlert()
{
	pushColor(alert);
}

void AppColors::pushError()
{
	pushColor(error);
}