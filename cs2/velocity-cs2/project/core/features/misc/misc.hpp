#pragma once

#include <limits>

#include <core/systems/systems.hpp>

namespace features::misc {

	class removals
	{
	public:
		void on_prepare_scene_material( std::uintptr_t material ) const;
		void on_override_view( std::uintptr_t view_setup ) const;
	};

	class camera
	{
	public:
		void on_override_view( std::uintptr_t view_setup );
		void update_fov_sensitivity( std::uintptr_t player_pawn ) const;

	private:
		void do_thirdperson( std::uintptr_t view_setup, std::uintptr_t local_pawn ) const;
		void do_fov_change( std::uintptr_t view_setup, std::uintptr_t local_pawn ) const;
		void do_aspect_ratio_change( std::uintptr_t view_setup );

		float m_original_aspect_ratio{};
		mutable float m_cached_fov_sensitivity{ -1.0f };
		mutable bool m_cached_scoped{};
		mutable float m_cached_target_fov{};
	};

	class hud
	{
	public:
		void on_render( xdraw::draw_list& draw_list );

	private:
		void do_crosshair( xdraw::draw_list& draw_list, float cx, float cy ) const;
		void do_scope( xdraw::draw_list& draw_list, float cx, float cy, float screen_h, std::uintptr_t local_pawn );
		void do_hat( xdraw::draw_list& draw_list, std::uintptr_t local_pawn ) const;
		void do_velocity( xdraw::draw_list& draw_list, float cx, float screen_h, std::uintptr_t local_pawn );

		static constexpr std::size_t k_velocity_history{ 120 };

		float m_scope_anim{};
		float m_spread_smooth{};
		std::uint32_t m_last_weapon{};
		float m_cached_spread_pixels{};
		int m_scope_update_frame{};
		std::array<float, k_velocity_history> m_velocity_history{};
		std::size_t m_velocity_history_count{};
		std::size_t m_velocity_history_head{};
		float m_velocity_smoothed{};
		float m_velocity_scale{};
	};

	class dlight
	{
	public:
		void on_present( );

	private:
		int m_connected_frames{};
	};

	class other
	{
	public:
		void on_round_start( );
		void on_frame_stage_notify( );
		void do_kill_feed_preservation( );
		void on_player_death( std::uintptr_t event );

		[[nodiscard]] bool is_alpha_changed( ) const { return this->m_is_alpha_changed; }

		static inline std::string s_display_name{};

	private:
		void do_autobuy( ) const;
		void do_player_alpha_changing( );
		void do_name_changing( ) const;
		void do_viewmodel_adjust( );

		bool m_is_alpha_changed{};
		float m_last_spawntime{};
		float m_cached_vm_x{ std::numeric_limits<float>::quiet_NaN( ) };
		float m_cached_vm_y{ std::numeric_limits<float>::quiet_NaN( ) };
		float m_cached_vm_z{ std::numeric_limits<float>::quiet_NaN( ) };
		float m_cached_vm_fov{ std::numeric_limits<float>::quiet_NaN( ) };
	};

	// this is so ghetto but fuck it for now it works


	struct c_panel_2d {

	};

	struct c_top_level_window_source_2 {

	};

	class c_ui_panel {
	public:
		void* m_vtable; //0x0000
		c_panel_2d* m_panel; //0x0008
		char* m_panel_name; //0x0010
		c_ui_panel* m_parent_panel; //0x0018
		c_top_level_window_source_2* m_ui_window; //0x0020
		int32_t m_child_ui_panel_count; //0x0028
		std::uint8_t pad_0000 [0x4]; //0x002C
		c_ui_panel** m_children_ui_panel_array; //0x0030
		std::uint8_t pad_0001 [0x18]; //0x0038
		float m_width; //0x0050
		float m_height; //0x0054
		std::uint8_t pad_0002 [0x270]; //0x0058
	};

	//class c_ui_engine {
	//public:
	//	void run_script (c_ui_panel* panel, const char* script) {
	//		const auto vmt = *reinterpret_cast<std::uintptr_t*>(this);
	//		const auto fn = *reinterpret_cast<std::uintptr_t*>(vmt + 77 * sizeof (std::uintptr_t));

	//		using fn_t = void (__thiscall*)(c_ui_engine*, c_ui_panel*, const char*, const char*, std::uint64_t);
	//		reinterpret_cast<fn_t>(fn)(this, panel, script, nullptr, 0);
	//	}
	//};

	struct panel_data_t {
		std::uint8_t pad_0000 [8];
		uint32_t m_flags;
		uint32_t m_visible;
		c_ui_panel* m_panel;
		uint32_t m_panel_id;
		std::uint8_t pad_0001 [4];
	};

	class c_ui_engine {
	public:
		void* m_vtable; //0x0000
		std::uint8_t pad_0000 [0x220]; //0x002C
		panel_data_t* m_panels_array; //0x0228
		int32_t m_panel_count; //0x0230
		std::uint8_t pad_0001 [0x3CC]; //0x002C
		void* m_isolate; //0x0600
		std::uint8_t pad_0002 [0x2E0]; //0x002C

		void* m_panel_stack [63]; //0x08E8
		int32_t m_panel_stack_depth; //0x0AE0
		std::uint8_t pad_0003 [0x8C]; //0x002C
		void* m_script_cache; //0x0B70
		std::uint8_t pad_0004 [0x10]; //0x002C

		void run_script (c_ui_panel* panel, const char* script);

	};

	class c_panorama_ui_engine {
	private:
		std::uint8_t pad_0001 [40]; //0x002C
		c_ui_engine* m_ui_engine; //0x0028
	public:
		c_ui_engine* get_ui_engine ();
	};

	class scoreboard_weapons {
	public:
		void on_frame_stage_notify ();
		void on_level_change ();

	private:
		struct weapon_entry {
			std::string name {};
			int         type {};

			bool operator==(const weapon_entry& o) const {
				return name == o.name && type == o.type;
			}
		};

		struct player_weapon_state {
			std::vector<weapon_entry> weapons {};
			std::string               active_name {};

			bool operator==(const player_weapon_state& o) const {
				return weapons == o.weapons && active_name == o.active_name;
			}
		};

		void try_initialize ();
		void run_script (const std::string& script);
		void send_player_weapons (std::uintptr_t controller);
		void send_clear (std::uint64_t steamid);
		void clear_all ();

		bool                                                 m_script_injected {};
		std::unordered_map<std::uint64_t, player_weapon_state> m_cache {};
		int                                                  m_throttle {};
		int                                                  m_init_throttle {};

		// panorama handles — resolved once per level, cleared on level change
		c_ui_engine* m_ui_engine {};
		c_ui_panel* m_scoreboard_panel {};
	};

} // namespace features::misc