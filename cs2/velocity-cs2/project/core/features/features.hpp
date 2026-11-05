#pragma once

// cosmetic-only build: visuals + client-side skin changer
#include "misc/misc.hpp"
#include "world/world.hpp"
#include "changer/changer.hpp"

namespace features {

	namespace misc {

		inline dlight g_dlight{};
		inline removals g_removals{};
		inline camera g_camera{};
		inline hud g_hud{};
		inline other g_other{};
		inline scoreboard_weapons g_scoreboard_weapons{};

	} // namespace misc

	namespace world {

		inline weather g_weather{};
		inline scene g_scene{};
		inline smoke g_smoke{};

	} // namespace world

	namespace changer {

		inline econ_item_system g_econ_item_system{};
		inline agents g_agents{};
		inline gloves g_gloves{};
		inline guns g_guns{};
		inline knives g_knives{};

	} // namespace changer

} // namespace features
