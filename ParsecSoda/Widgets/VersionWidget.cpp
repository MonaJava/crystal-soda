#include "VersionWidget.h"

// Constructor
VersionWidget::VersionWidget() {
	version = Cache::cache.version;
	latestVersion = "";
	changeLog = "";
	error = "";
    strcpy_s(_email, "");
    strcpy_s(_password, "");
    strcpy_s(_2fa, "");
}

bool VersionWidget::render() {

    static ImVec2 res;
    static ImVec2 cursor;

    res = ImGui::GetMainViewport()->Size;
    cursor = ImGui::GetCursorPos();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0));

    ImGui::SetNextWindowPos(ImVec2(10, res.y - 30));
    ImGui::SetNextWindowSize(ImVec2(240, 52));
    ImGui::Begin("##Version", (bool*)0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);
    AppStyle::pushInput();
    ImGui::Text("FPS: ");
    AppStyle::pop();
    ImGui::SameLine();
    AppStyle::pushPositive();
    ImGui::Text("%.0f", ImGui::GetIO().Framerate);
    AppStyle::pop();

    ImGui::SameLine();

    AppStyle::pushInput();
    std::string versionString = "Smash Soda v. ";
    ImGui::Text(versionString.c_str());
    AppStyle::pop();

    ImGui::SameLine();
    AppStyle::pushPositive();
	ImGui::Text(version.c_str());
    
    AppStyle::pop();
    ImGui::End();

    //ImGui::PopStyleColor();
    //ImGui::PopStyleColor();
    //ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    return true;
}

/// <summary>
/// Login window for Soda Arcade
/// </summary>
/// <returns></returns>
bool VersionWidget::renderLoginWindow() {

    AppStyle::pushTitle();

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 580);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Soda Arcade###Soda Arcade Login Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    AppStyle::pushInput();
    AppColors::pushButtonSolid();

    // Center image
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 150);
    ImGui::Image(AppIcons::sodaArcadeLogo, ImVec2(300, 70));

    // Text with wrap
    AppStyle::pushSmall();
	ImGui::TextWrapped("Soda Arcade is a new service for advertising your Parsec room when you mark it as public. You will need to create a account to use this service.");
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 48);
    if (ImGui::Button("Create account")) {
		const wchar_t* link = L"https://soda-arcade.com/register";
        ShellExecute(0, 0, link, 0, 0, SW_SHOW);
    }
    
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 96);

    if (ImGui::Button("I don't want to use Soda Arcade")) {
		Config::cfg.arcade.showLogin = false;
        Config::cfg.room.privateRoom = true;
		Config::cfg.Save();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

	ImGui::Dummy(ImVec2(0, 10));

	if (ImForm::InputText("EMAIL ADDRESS", _email, "The e-mail address you signed up to Soda Arcade with.")) {
        	
    }

	if (ImForm::InputPassword("PASSWORD", _password, "The password you signed up to Soda Arcade with.")) {

	}

    if (ImForm::InputText("2FA CODE", _2fa, "2FA code if you have it enabled on your account.")) {

    }

	ImGui::Spacing();

    if (ImGui::Button("Login")) {

		error = "";

		std::string email = _email;
		std::string password = _password;
        std::string twoFactor = _2fa;

        if (Arcade::instance.login(email, password, twoFactor)) {
            Config::cfg.arcade.showLogin = false;
            Config::cfg.Save();
		} else {
			error = "Invalid e-mail or password.";
		}

    }

    ImGui::Spacing();

	if (error.length() > 0) {
		ImGui::TextWrapped(error.c_str());
    }

    AppColors::popButton();
    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;

}

/// <summary>
/// Renders the overlay download window
/// </summary>
/// <returns></returns>
bool VersionWidget::renderDownloadWindow() {

    AppStyle::pushTitle();

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 200);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Update Overlay###Overlay Download Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    AppStyle::pushInput();
    AppColors::pushButtonSolid();

    // Text with wrap
    AppStyle::pushSmall();
    ImGui::TextWrapped("The overlay files need to be updated. Delete the contents of the overlay folder wherever you have Smash Soda installed, and download the new files.");
    AppStyle::pop();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 35);
    if (ImGui::Button("Download")) {
        const wchar_t* link = L"https://github.com/MickeyUK/smash-soda-overlay/releases/latest";
        ShellExecute(0, 0, link, 0, 0, SW_SHOW);
        Config::cfg.overlay.update = false;
        Config::cfg.Save();
        showDownload = false;
    }

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 85);
    if (ImGui::Button("I've already downloaded")) {
        showDownload = false;
        Config::cfg.overlay.update = false;
        Config::cfg.Save();
    }

    AppColors::popButton();
    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;

}

/// <summary>
/// Renders the update log window
/// </summary>
/// <returns></returns>
bool VersionWidget::renderUpdateWindow() {

    AppStyle::pushTitle();

    static ImVec2 res;
    static ImVec2 size = ImVec2(400, 500);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Update Available###Update Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    AppStyle::pushInput();

	AppStyle::pushPositive();
	ImGui::Text("Version: ");
    ImGui::SameLine();
    ImGui::Text(Cache::cache.update.version.c_str());
    AppStyle::pop();

    // Text with wrap
    AppStyle::pushSmall();
	ImGui::TextWrapped("A new version of Smash Soda is available. Please close this, pull the latest commit and rebuild!");
    AppStyle::pop();
    
    ImGui::Spacing();
	ImGui::Separator();
    ImGui::Spacing();
    

    ImGui::Markdown(Cache::cache.update.notes.c_str(), Cache::cache.update.notes.length(), mdConfig);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    AppColors::pushButtonSolid();
    
    if (!Cache::cache.update.critical) {
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50);
        if (ImGui::Button("Close")) {
            showUpdate = false;
        }
        AppColors::popButton();
    }

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;

}