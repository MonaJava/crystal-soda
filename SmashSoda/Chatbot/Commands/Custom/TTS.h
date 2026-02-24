#pragma once
#pragma once

#include "../../ACommand.h"
#include "../../../GuestList.h"

#include <sapi.h>
#include <sstream>


class TTS : public ACommand
{
public:
	/**
	 * @brief Construct a new CommandAFK object
	 *
	 * @param guests
	 * @param padClient
	 */
	TTS(const char* msg, Guest& sender)
		: ACommand(msg, sender), _sender(sender)
	{}

	/**
	 * @brief Run the command
	 * @return true if the command was successful
	 */
	bool run() override {

		if (getArgs().size() == 0) {
			setReply("Usage: /tts <string>\0");
			return false;
		}

		ISpVoice * pVoice = NULL;

		if (FAILED(::CoInitialize(NULL)))
			return false;

		string _stringArg = getArgString();
		std::string narrow = _stringArg;
		std::wstring wide = L"";
		std::wstringstream cls;
		cls << narrow.c_str() << wide.c_str();
		std::wstring total = cls.str();
		const wchar_t *test = total.c_str();


		HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
		if( SUCCEEDED( hr ) )
		{
		
		
			hr = pVoice->Speak(test, 0, NULL);
			pVoice->Release();
			pVoice = NULL;
		}

		::CoUninitialize();
			return true;
		}

	/**
	 * @brief Get the prefixes object
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "/tts" };
	}
protected:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "/tts " };
	}
	Guest& _sender;
};

