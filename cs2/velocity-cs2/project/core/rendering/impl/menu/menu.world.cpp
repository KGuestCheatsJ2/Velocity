#include <pch/pch.hpp>
#include <core/settings.hpp>

#include "../../rendering.hpp"

namespace rendering {

	void menu::draw_world( float group_w ) const
	{
		auto& w = settings::g_world;

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
			auto& scene = w.m_scene;

			if ( xui::begin_child( "##world_scene_left", col_w ) )
			{
				xui::checkbox( "skybox material", scene.skybox.custom_skybox );
				if ( xui::begin_popup( "##skybox_popup", 220.0f ) )
				{
					xui::slider_int( "skybox id", scene.skybox.selected_skybox, 0, 60, "%d" );
					xui::end_popup( );
				}

				xui::checkbox( "skybox color", scene.skybox.custom_color );
				if ( xui::begin_popup( "##skycolor_popup", 220.0f ) )
				{
					xui::color_picker( "sky color", scene.skybox.skybox_color );
					xui::color_picker( "cloud color", scene.skybox.cloud_color );
					xui::color_picker( "sun color", scene.skybox.sun_color );
					xui::end_popup( );
				}

				xui::checkbox( "world color", scene.world_setting );
				if ( xui::begin_popup( "##worldcolor_popup", 220.0f ) )
				{
					xui::color_picker( "color##world", scene.world_color );
					xui::end_popup( );
				}

				xui::checkbox( "lighting", scene.lighting );
				if ( xui::begin_popup( "##lighting_popup", 220.0f ) )
				{
					xui::slider_float( "intensity##light", scene.lighting_intensity, 0.0f, 2.0f, "%.2f" );
					xui::color_picker( "color##light", scene.lighting_color );
					xui::end_popup( );
				}

				xui::checkbox( "bloom", scene.bloom );
				if ( xui::begin_popup( "##bloom_popup", 220.0f ) )
				{
					xui::slider_float( "value##bloom", scene.bloom_value, 0.0f, 2.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::checkbox( "gamma", scene.gamma );
				if ( xui::begin_popup( "##gamma_popup", 220.0f ) )
				{
					xui::slider_float( "value##gamma", scene.gamma_value, 0.5f, 5.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "##world_scene_right", col_w ) )
			{
				xui::checkbox( "depth of field", scene.dof );
				if ( xui::begin_popup( "##dof_popup", 220.0f ) )
				{
					xui::slider_float( "near blurry", scene.dof_near_blurry, 0.0f, 50.0f, "%.0f" );
					xui::slider_float( "near crisp", scene.dof_near_crisp, 0.0f, 100.0f, "%.0f" );
					xui::slider_float( "far crisp", scene.dof_far_crisp, 100.0f, 2000.0f, "%.0f" );
					xui::slider_float( "far blurry", scene.dof_far_blurry, 200.0f, 5000.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::checkbox( "ambient", scene.ambient );
				if ( xui::begin_popup( "##ambient_popup", 220.0f ) )
				{
					xui::slider_float( "intensity##amb", scene.ambient_intensity, 0.0f, 3.0f, "%.1f" );
					xui::color_picker( "color##amb", scene.ambient_color );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}

		if ( subtab == 1 )
		{
			auto& weather = w.m_weather;

			if ( xui::begin_child( "##world_weather", col_w ) )
			{
				xui::checkbox( "weather", weather.enabled );
				if ( xui::begin_popup( "##weather_popup", 220.0f ) )
				{
					constexpr const char* weather_types[ ]{ "snow", "rain", "stars" };
					xui::combo( "type", weather.type.value, weather_types, 3 );

					xui::color_picker( "color##weather", weather.color );
					xui::end_popup( );
				}

				xui::checkbox( "fog", weather.fog_enabled );
				if ( xui::begin_popup( "##fog_popup", 220.0f ) )
				{
					xui::slider_float( "density##fog", weather.fog_density, 0.0f, 1.0f, "%.2f" );
					xui::slider_float( "anisotropy", weather.fog_anisotropy, 0.0f, 1.0f, "%.2f" );
					xui::slider_float( "draw distance", weather.fog_draw_distance, 500.0f, 20000.0f, "%.0f" );
					xui::color_picker( "color##fog", weather.fog_color );
					xui::end_popup( );
				}

				xui::checkbox( "wetness", weather.wetness );
				if ( xui::begin_popup( "##wetness_popup", 220.0f ) )
				{
					xui::slider_float( "density##wet", weather.wetness_density, 0.0f, 5.0f, "%.1f" );
					xui::slider_float( "speed##wet", weather.wetness_speed, 0.0f, 3.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::checkbox( "wind", weather.wind );
				if ( xui::begin_popup( "##wind_popup", 220.0f ) )
				{
					xui::slider_float( "strength##wind", weather.wind_strength, 0.0f, 5.0f, "%.1f" );
					xui::slider_float( "frequency##wind", weather.wind_frequency, 0.0f, 10.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}
	}

} // namespace rendering