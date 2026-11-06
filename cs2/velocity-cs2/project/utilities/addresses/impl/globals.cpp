#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/logging/logging.hpp>
#include <protection/game_addresses.hpp>
#include "../addresses.hpp"

namespace addresses::globals {

	bool initialize () {
		source2client              = INTERFACE_ ("Source2Client002");
		panorama = INTERFACE_ ("PanoramaUIEngine001");
		source2engine_to_client    = INTERFACE_ ("Source2EngineToClient001");
		scene_system               = INTERFACE_ ("SceneSystem_002");
		material_system            = INTERFACE_ ("VMaterialSystem2_001");
		schema_system              = INTERFACE_ ("SchemaSystem_001");
		input_system               = INTERFACE_ ("InputSystemVersion001");
		particle_system_mgr        = INTERFACE_ ("ParticleSystemMgr003");
		cvar                       = (interfaces::c_engine_cvar*)INTERFACE_ ("VEngineCvar007");
		source2client_prediction   = INTERFACE_ ("Source2ClientPrediction001");
		network_client_service     = INTERFACE_ ("NetworkClientService_001");
		resource_system            = INTERFACE_ ("ResourceSystem013");
		localize                   = INTERFACE_ ("Localize_001");
		mesh_system                = INTERFACE_ ("MeshSystem001");
		file_system                = INTERFACE_ ("VFileSystem017");

		csgo_input             = PATTERN (patterns::csgo_input);
		entity_list            = PATTERN (patterns::entity_list);
		local_player_controller = PATTERN (patterns::local_player_controller);
		global_vars            = PATTERN (patterns::global_vars);
		view_matrix            = PATTERN (patterns::view_matrix);
		game_rules             = PATTERN (patterns::game_rules);
		light_data_queue       = PATTERN (patterns::light_data_queue);
		particle_manager       = PATTERN (patterns::particle_manager);
		game_event_manager     = PATTERN (patterns::game_event_manager);
		game_trace_manager     = PATTERN (patterns::game_trace_manager);
		render_game_system     = PATTERN (patterns::render_game_system);
		material_manager       = PATTERN (patterns::material_manager);
		game_entity_system     = PATTERN (patterns::game_entity_system);
		weapon_recoil_data     = PATTERN (patterns::weapon_recoil_data);
		hud                    = PATTERN (patterns::hud);
		prediction_seed        = PATTERN (patterns::prediction_seed);
		simulation_player      = PATTERN (patterns::simulation_player);
		prediction_player      = PATTERN (patterns::prediction_player);
		planted_c4             = PATTERN (patterns::planted_c4);
		item_system            = PATTERN (patterns::item_system);
		net_client             = PATTERN (patterns::net_client);
		frame_input_ring_idx   = PATTERN (patterns::frame_input_ring_idx);
		frame_input_ring_base  = PATTERN (patterns::frame_input_ring_base);
		prediction_state       = PATTERN (patterns::prediction_state);

		if (const auto ptr = MODULE_EXPORT("tier0.dll:g_pMemAlloc"))
			mem_alloc = *reinterpret_cast<std::uintptr_t*>(ptr);

		{
			const auto storage = PATTERN (patterns::swap_chain_storage);
			if (!storage) {
				logging::console::print ("[globals] swap_chain_storage pattern not found");
				return false;
			}

			const auto first = memory::read<std::uintptr_t> (storage);
			if (!first) {
				logging::console::print ("[globals] swap chain: first pointer is null");
				return false;
			}

			const auto second = memory::read<std::uintptr_t> (first);
			if (!second) {
				logging::console::print ("[globals] swap chain: second pointer is null");
				return false;
			}

			swap_chain = memory::read<std::uintptr_t> (second + 0x170);
		}

		// patterns that went stale after a game update only disable the feature that needs them
		// (weather: particle_manager, skin changer: item_system, game events: game_event_manager)
		const struct { const char* name; std::uintptr_t value; } optional_globals[] {
			{ "particle_manager", particle_manager }, { "game_event_manager", game_event_manager },
			{ "weapon_recoil_data", weapon_recoil_data }, { "simulation_player", simulation_player },
			{ "planted_c4", planted_c4 }, { "item_system", item_system },
		};
		for (const auto& g : optional_globals) {
			if (!g.value) {
				logging::console::print ("[globals] optional global '{}' not found, related feature disabled", g.name);
			}
		}

		const struct { const char* name; std::uintptr_t value; } required_globals[] {
			{ "csgo_input", csgo_input }, { "entity_list", entity_list }, { "local_player_controller", local_player_controller },
			{ "global_vars", global_vars }, { "view_matrix", view_matrix }, { "game_rules", game_rules },
			{ "light_data_queue", light_data_queue }, { "game_trace_manager", game_trace_manager },
			{ "render_game_system", render_game_system }, { "mem_alloc", mem_alloc }, { "swap_chain", swap_chain },
			{ "material_manager", material_manager }, { "game_entity_system", game_entity_system }, { "hud", hud },
			{ "prediction_seed", prediction_seed }, { "prediction_player", prediction_player }, { "net_client", net_client },
			{ "frame_input_ring_idx", frame_input_ring_idx }, { "frame_input_ring_base", frame_input_ring_base },
			{ "prediction_state", prediction_state },
		};

		auto all_found = true;
		for (const auto& g : required_globals) {
			if (!g.value) {
				logging::console::print ("[globals] REQUIRED global '{}' is missing", g.name);
				all_found = false;
			}
		}

		if (!all_found) {
			return false;
		}

		return true;
	}

} // namespace addresses::globals
