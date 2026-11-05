#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include "../logging.hpp"

namespace logging::console {

	namespace detail {

		std::uintptr_t log_direct_color {};
		int log_general {};

		static inline void lerp_color (const std::uint8_t* from, const std::uint8_t* to, float t, std::uint8_t* out) {
			for (auto i = 0; i < 4; ++i) {
				out [i] = static_cast<std::uint8_t> (from [i] + (to [i] - from [i]) * t);
			}
		}

	} // namespace detail

	bool initialize () {
		// resolve by name: ordinals shift between game updates and caused a crash inside tier0
		// (direct GetProcAddress on purpose, so a failure here can never recurse into logging)
		const auto tier0 = GetModuleHandleA ("tier0.dll");
		if (!tier0) {
			return false;
		}

		detail::log_direct_color = reinterpret_cast<std::uintptr_t>(GetProcAddress (tier0, "LoggingSystem_Log"));
		if (!detail::log_direct_color) {
			return false;
		}

		const auto log_general = reinterpret_cast<const int*>(GetProcAddress (tier0, "LOG_GENERAL"));
		if (!log_general) {
			detail::log_direct_color = 0;
			return false;
		}

		detail::log_general = *log_general;

		return true;
	}

	void print_raw (const char* text) {
		if (!detail::log_direct_color) {
			return;
		}

		emitting = true;

		constexpr auto prefix {"[velocity] "};
		constexpr auto prefix_len {11ull};

		constexpr std::uint8_t pink [4] {252, 217, 240, 255};
		constexpr std::uint8_t blue [4] {173, 192, 255, 255};
		constexpr std::uint8_t blite [4] {220, 225, 240, 255};

		char buf [2] {0, 0};

		for (auto i = 0ull; i < prefix_len; ++i) {
			const auto t = prefix_len > 1 ? static_cast<float>(i) / (prefix_len - 1) : 0.f;

			std::uint8_t color [4];
			detail::lerp_color (pink, blue, t, color);

			buf [0] = prefix [i];
			memory::call<int> (detail::log_direct_color, detail::log_general, 2, *reinterpret_cast<int*>(&color), buf);
		}

		char msg [2048];
		std::snprintf (msg, sizeof (msg), "%s\n", text);

		memory::call<int> (detail::log_direct_color, detail::log_general, 2, *reinterpret_cast<int*>(const_cast<std::uint8_t*>(blite)), msg);

		emitting = false;
	}

} // namespace logging::console