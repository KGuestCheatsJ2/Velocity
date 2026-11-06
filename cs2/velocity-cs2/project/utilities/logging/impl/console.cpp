#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "../logging.hpp"

namespace logging::console {

	// The game's internal LoggingSystem_Log changed its signature in a recent update (the color argument
	// is now read through a pointer), which crashed inside tier0. Messages are therefore no longer routed
	// through the game console; they go to the debugger and to %TEMP%\velocity_dev.log instead.

	bool initialize () {
		return true;
	}

	void print_raw (const char* text) {
		if (!text) {
			return;
		}

		emitting = true;

		OutputDebugStringA ("[velocity] ");
		OutputDebugStringA (text);
		OutputDebugStringA ("\n");

		char path [MAX_PATH] {};
		const auto len = GetTempPathA (static_cast<DWORD> (sizeof (path)), path);
		if (len > 0 && len < sizeof (path) - 32) {
			strcat_s (path, "velocity_dev.log");

			const auto file = CreateFileA (path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (file != INVALID_HANDLE_VALUE) {
				DWORD written {};
				WriteFile (file, text, static_cast<DWORD> (std::strlen (text)), &written, nullptr);
				WriteFile (file, "\r\n", 2, &written, nullptr);
				CloseHandle (file);
			}
		}

		emitting = false;
	}

} // namespace logging::console
