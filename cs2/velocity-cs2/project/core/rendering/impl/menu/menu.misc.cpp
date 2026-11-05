#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		constexpr const char* sound_types[ ]{ "shop click", "home click", "bell", "killcard", "bullet casing", "coin pickup", "item drop", "popcan", "key press", "custom" };
		constexpr auto k_sound_type_count{ static_cast< int >( std::size( sound_types ) ) };

		constexpr const char* marker_types[ ]{ "classic", "damage", "both" };
		constexpr const char* impact_types[ ]{ "overlay", "sparks", "both" };

		constexpr const char* primary_weapons[ ]{ "none", "rifle", "scoped rifle", "scout", "awp", "auto sniper" };
		constexpr const char* secondary_weapons[ ]{ "none", "dual elites", "five-seven/tec-9", "deagle", "revolver" };
		constexpr const char* grenade_names[ ]{ "molotov", "he grenade", "smoke", "flashbang", "decoy" };

		constexpr const char* nc_modes[ ]{ "clantag", "override name" };
		constexpr const char* nc_animations[ ]{ "typewriter", "none" };

		constexpr const char* hat_types[ ]{ "kasa", "bucket" };

	} // namespace detail

	void menu::draw_misc( float group_w ) const
	{
		auto& m = settings::g_misc;

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = wx + tokens::gap + tokens::sidebar_w + tokens::gap;
		const auto body_y = wy + tokens::gap + tokens::subtab_bar_h + tokens::gap;
		const auto content_w = this->m_w - tokens::gap * 2.0f - tokens::sidebar_w - tokens::gap;
		const auto col_w = ( content_w - tokens::gap ) * 0.5f;
		const auto right_x = content_x + col_w + tokens::gap;

		const auto subtab = this->m_subtab;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( subtab == 0 )
		{
			auto& dlights = m.m_dlight;
			auto& ab = m.m_autobuy;

			if ( xui::begin_child( "##misc_visuals", col_w ) )
			{
				xui::checkbox( "dynamic light", dlights.enabled );
				if ( xui::begin_popup( "##dlight_popup", 220.0f ) )
				{
					xui::color_picker( "color##dl", dlights.color );
					xui::slider_float( "radius##dl", dlights.radius, 50.0f, 15000.0f, "%.0f" );
					xui::slider_float( "z offset##dl", dlights.z_offset, 0.0f, 100.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "##misc_other", col_w ) )
			{
				xui::checkbox( "preserve killfeed", m.preserve_killfeed );
				xui::checkbox( "disable game logs", m.disable_game_logs );

				xui::checkbox( "auto buy", ab.enabled );
				if ( xui::begin_popup( "##autobuy_popup", 220.0f ) )
				{
					xui::combo( "primary##ab", ab.primary_weapon, detail::primary_weapons, 6 );
					xui::combo( "secondary##ab", ab.secondary_weapon, detail::secondary_weapons, 5 );
					xui::checkbox( "armor##ab", ab.armor );
					xui::checkbox( "defuser##ab", ab.defuser );
					xui::checkbox( "taser##ab", ab.taser );
					xui::multicombo( "grenades##ab", ab.grenades, detail::grenade_names, 5 );
					xui::end_popup( );
				}

				xui::checkbox( "name changer", m.m_name_changer.enabled );
				if ( xui::begin_popup( "##namechanger_popup", 220.0f ) )
				{
					xui::combo( "mode##nc", m.m_name_changer.mode.value, detail::nc_modes, 2 );
					xui::combo( "animation##nc", m.m_name_changer.animation.value, detail::nc_animations, 2 );
					xui::slider_float( "speed##nc", m.m_name_changer.speed, 0.05f, 1.0f, "%.2fs" );
					xui::text_input( "text##nc", m.m_name_changer.text.value, 32, "tag text..." );
					xui::end_popup( );
				}

				xui::checkbox( "watermark", m.m_watermark.enabled );
				if ( xui::begin_popup( "##watermark_popup", 200.0f ) )
				{
					xui::checkbox( "fps##wm",      m.m_watermark.show_fps );
					xui::checkbox( "ping##wm",     m.m_watermark.show_ping );
					xui::checkbox( "time##wm",     m.m_watermark.show_time );
					xui::checkbox( "user##wm",     m.m_watermark.show_user );
					xui::checkbox( "map##wm",      m.m_watermark.show_map );
					xui::checkbox( "tick rate##wm",m.m_watermark.show_tick );
					xui::checkbox( "velocity##wm", m.m_watermark.show_velocity );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}

		if ( subtab == 1 )
		{
			auto& rem = m.m_removals;

			if ( xui::begin_child( "##misc_removals", col_w ) )
			{
				xui::checkbox( "remove crosshair", rem.crosshair );
				xui::checkbox( "remove scope", rem.scope );
				xui::checkbox( "remove overhead", rem.overhead );
				xui::checkbox( "remove legs", rem.legs );
				xui::checkbox( "remove recoil", rem.recoil );
				xui::checkbox( "remove skybox fog", rem.skybox_fog );
				xui::checkbox( "remove 3d skybox", rem.skybox_3d );
				xui::checkbox( "remove decals", rem.decals );

				xui::end_child( );
			}
		}

		if ( subtab == 2 )
		{
			auto& cam = m.m_camera;
			auto& vm = m.m_viewmodel_adjust;

			if ( xui::begin_child( "##misc_camera", col_w ) )
			{
				xui::checkbox( "custom fov", cam.change_fov );
				if ( xui::begin_popup( "##fov_popup", 220.0f ) )
				{
					xui::slider_float( "fov", cam.fov, 60.0f, 150.0f, "%.0f" );
					xui::checkbox( "scoped fov override", cam.scoped_fov_override );
					xui::slider_float( "scoped fov", cam.scoped_fov, 10.0f, 90.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::checkbox( "thirdperson", cam.thirdperson );
				if ( xui::begin_popup( "##tp_popup", 220.0f ) )
				{
					xui::slider_float( "distance", cam.thirdperson_distance, 35.0f, 200.0f, "%.0f" );
					xui::slider_float( "hull size", cam.thirdperson_hull_size, 0.0f, 20.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::checkbox( "aspect ratio", cam.change_aspect_ratio );
				if ( xui::begin_popup( "##ar_popup", 220.0f ) )
				{
					xui::slider_float( "ratio##ar", cam.aspect_ratio, 1.0f, 1.78f, "%.3f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "##misc_viewmodel", col_w ) )
			{
				xui::checkbox( "viewmodel adjust", vm.enabled );
				if ( xui::begin_popup( "##vm_popup", 220.0f ) )
				{
					xui::slider_float( "offset x", vm.offset_x, -10.0f, 10.0f, "%.1f" );
					xui::slider_float( "offset y", vm.offset_y, -10.0f, 10.0f, "%.1f" );
					xui::slider_float( "offset z", vm.offset_z, -10.0f, 10.0f, "%.1f" );
					xui::slider_float( "fov", vm.fov, 54.0f, 90.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}

		if ( subtab == 3 )
		{
			auto& hud = m.m_hud;

			if ( xui::begin_child( "##misc_hud", col_w ) )
			{
				xui::checkbox( "crosshair overlay", hud.m_crosshair.enabled );
				if ( xui::begin_popup( "##xhair_popup", 220.0f ) )
				{
					xui::slider_float( "size##xhair", hud.m_crosshair.size, 0.5f, 10.0f, "%.1f" );
					xui::slider_float( "outline##xhair", hud.m_crosshair.outline, 0.0f, 4.0f, "%.1f" );
					xui::color_picker( "color##xhair", hud.m_crosshair.color );
					xui::color_picker( "outline color##xhair", hud.m_crosshair.outline_color );
					xui::end_popup( );
				}

				xui::checkbox( "scope overlay", hud.m_scope.enabled );
				if ( xui::begin_popup( "##scope_popup", 220.0f ) )
				{
					xui::slider_float( "line length", hud.m_scope.line_length, 10.0f, 500.0f, "%.0f" );
					xui::slider_float( "gap##scope", hud.m_scope.gap, 0.0f, 50.0f, "%.0f" );
					xui::slider_float( "thickness##scope", hud.m_scope.thickness, 0.5f, 5.0f, "%.2f" );
					xui::slider_float( "anim speed", hud.m_scope.anim_speed, 1.0f, 30.0f, "%.0f" );
					xui::color_picker( "color##scope", hud.m_scope.color );
					xui::checkbox( "fade in##scope", hud.m_scope.fade_in );

					xui::layout::separator( );

					xui::checkbox( "glow##scope", hud.m_scope.glow );
					xui::slider_float( "glow strength##scope", hud.m_scope.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::checkbox( "velocity counter", hud.m_velocity.counter );
				xui::checkbox( "velocity chart", hud.m_velocity.chart );
				if ( xui::begin_popup( "##velocity_hud_popup", 220.0f ) )
				{
					xui::color_picker( "color##velocity", hud.m_velocity.color );
					xui::slider_float( "bottom offset", hud.m_velocity.bottom_offset, 20.0f, 200.0f, "%.0f" );
					xui::slider_float( "chart width", hud.m_velocity.chart_width, 120.0f, 320.0f, "%.0f" );
					xui::slider_float( "chart height", hud.m_velocity.chart_height, 24.0f, 80.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "##misc_hud_hat", col_w ) )
			{
				xui::checkbox( "hat", hud.m_hat.enabled );
				if ( xui::begin_popup( "##hat_popup", 220.0f ) )
				{
					xui::combo( "type##hat", hud.m_hat.type.value, detail::hat_types, 2 );
					xui::color_picker( "color##hat", hud.m_hat.color );
					xui::color_picker( "secondary color##hat", hud.m_hat.secondary_color );
					xui::checkbox( "glow##hat", hud.m_hat.glow );
					xui::slider_float( "glow strength##hat", hud.m_hat.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}
	}

} // namespace rendering