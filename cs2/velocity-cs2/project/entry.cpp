#include <pch/pch.hpp>
#include <cstdio>
#include <cstring>

#include <utilities/logging/logging.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/threadpool/threadpool.hpp>
#include <utilities/bootstrap/bootstrap.hpp>
#include <utilities/steam/steam.hpp>

#include <core/hooks/hooks.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <core/rendering/rendering.hpp>
#include <protection/protection.hpp>

namespace {

#if defined( DEV )
	void init_log( const char* msg )
	{
		OutputDebugStringA( "[velocity] " );
		OutputDebugStringA( msg );
		OutputDebugStringA( "\n" );

		// also write to %TEMP%\velocity_dev.log so failures are visible without a debugger
		char path[ MAX_PATH ]{};
		if ( GetTempPathA( MAX_PATH, path ) )
		{
			strcat_s( path, "velocity_dev.log" );
			HANDLE f = CreateFileA( path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
			if ( f != INVALID_HANDLE_VALUE )
			{
				DWORD w{};
				WriteFile( f, msg, static_cast< DWORD >( strlen( msg ) ), &w, nullptr );
				WriteFile( f, "\r\n", 2, &w, nullptr );
				CloseHandle( f );
			}
		}
	}

	#define INIT_FAIL( msg ) \
		do { \
			init_log( msg ); \
			return 0; \
		} while ( 0 )

	#define INIT_WARN( msg ) init_log( msg )
	#define STAGE( name ) init_log( "stage: " name )

	LONG CALLBACK crash_logger( PEXCEPTION_POINTERS ep )
	{
		const auto code = ep->ExceptionRecord->ExceptionCode;
		if ( code != 0xC0000005 && code != 0xC000001D && code != 0xC0000094 && code != 0xC0000096 && code != 0xC0000409 && code != 0xC00000FD )
			return EXCEPTION_CONTINUE_SEARCH;

		static volatile LONG count = 0;
		if ( InterlockedIncrement( &count ) > 25 )
			return EXCEPTION_CONTINUE_SEARCH;

		const auto rip = reinterpret_cast< const void* >( ep->ExceptionRecord->ExceptionAddress );
		HMODULE mod{};
		char name[ MAX_PATH ]{};
		GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast< LPCSTR >( rip ), &mod );
		if ( mod )
			GetModuleFileNameA( mod, name, MAX_PATH );

		char buf[ 512 ]{};
		snprintf( buf, sizeof( buf ), "EXCEPTION code=0x%08lX rip=%p module=%s offset=0x%llX tid=%lu info0=%llu info1=0x%llX",
			code, rip, name[ 0 ] ? name : "?", static_cast< unsigned long long >( reinterpret_cast< const char* >( rip ) - reinterpret_cast< const char* >( mod ) ),
			GetCurrentThreadId( ), static_cast< unsigned long long >( ep->ExceptionRecord->NumberParameters > 0 ? ep->ExceptionRecord->ExceptionInformation[ 0 ] : 0 ),
			static_cast< unsigned long long >( ep->ExceptionRecord->NumberParameters > 1 ? ep->ExceptionRecord->ExceptionInformation[ 1 ] : 0 ) );
		init_log( buf );
		return EXCEPTION_CONTINUE_SEARCH;
	}
#else
	#define STAGE( name ) ( ( void )0 )
	#define INIT_FAIL( msg ) \
		do { \
			MessageBoxA( nullptr, xs( msg ), xs( "..." ), MB_ICONERROR ); \
			return 0; \
		} while ( 0 )

	#define INIT_WARN( msg ) INIT_FAIL( msg )
#endif

	DWORD WINAPI init_thread( LPVOID param )
	{
		const auto module_handle = static_cast<HMODULE>( param );

#if !defined( DEV )
		// bool protection_result = false; g_protection.attach( &protection_result );
		// if ( !protection_result )
		// {
		// 	g_protection.crash( );
		// 	return 0;
		// }
#endif

		STAGE( "CoInitializeEx" );
		CoInitializeEx( nullptr, COINIT_MULTITHREADED );

		STAGE( "config::initialize" );
		config::initialize( );
		STAGE( "settings::finalize_binds" );
		settings::finalize_binds( );

		STAGE( "bootstrap::on_dll_attach" );
		bootstrap::on_dll_attach( );


		{
			STAGE( "logging::console::initialize" );
			if ( !logging::console::initialize( ) )
			{
#if defined( DEV )
				INIT_WARN( "failed to initialize console logging." );
#else
				INIT_FAIL( "failed to initialize console logging." );
#endif
			}

			STAGE( "logging::popup::initialize" );
			if ( !PROTECTION_CHECK( ) || !logging::popup::initialize( ) )
			{
#if defined( DEV )
				INIT_WARN( "failed to initialize popup logging." );
#else
				INIT_FAIL( "failed to initialize popup logging." );
#endif
			}
		}

		{
			// module integrity cache removed: it only fed the disabled VAC hooks and called a stale game pattern (crash)

			STAGE( "threadpool::initialize" );
			if ( !PROTECTION_CHECK( ) || !threadpool::initialize( ) )
			{
				INIT_FAIL( "failed to initialize thread pool." );
			}

			STAGE( "steam::http::initialize" );
			if ( !PROTECTION_CHECK( ) || !steam::http::initialize( ) )
			{
				INIT_FAIL( "failed to initialize steam http." );
			}

			STAGE( "steam::friends::initialize" );
			if ( !PROTECTION_CHECK( ) || !steam::friends::initialize( ) )
			{
				INIT_FAIL( "failed to initialize steam friends." );
			}

			STAGE( "steam::utils::initialize" );
			if ( !PROTECTION_CHECK( ) || !steam::utils::initialize( ) )
			{
				INIT_FAIL( "failed to initialize steam utils." );
			}
		}

		{
			STAGE( "addresses::modules::initialize" );
			if ( !PROTECTION_CHECK( ) || !addresses::modules::initialize( ) )
			{
				INIT_FAIL( "failed to initialize module addresses." );
			}

			STAGE( "addresses::globals::initialize" );
			if ( !PROTECTION_CHECK( ) || !addresses::globals::initialize( ) )
			{
				INIT_FAIL( "failed to initialize global addresses." );
			}

			STAGE( "addresses::functions::initialize" );
			if ( !PROTECTION_CHECK( ) || !addresses::functions::initialize( ) )
			{
				INIT_FAIL( "failed to initialize function addresses." );
			}
		}

		{
			STAGE( "systems::materials::initialize" );
			if ( !PROTECTION_CHECK( ) || !systems::materials::initialize( ) )
			{
				INIT_FAIL( "failed to initialize materials system." );
			}

			STAGE( "systems::events::initialize" );
			if ( !PROTECTION_CHECK( ) || !systems::events::initialize( ) )
			{
				INIT_FAIL( "failed to initialize event system." );
			}

			STAGE( "PROTECTION_CHECK" );
			if ( !PROTECTION_CHECK( ) || !systems::g_icons.initialize( ) )
			{
				INIT_FAIL( "failed to initialize vpk parse system." );
			}

			STAGE( "PROTECTION_CHECK" );
			if ( !PROTECTION_CHECK( ) || !systems::g_model_preview.initialize( ) )
			{
				INIT_FAIL( "failed to initialize model preview system." );
			}
		}

		{
			STAGE( "PROTECTION_CHECK" );
			if ( !PROTECTION_CHECK( ) || !features::changer::g_econ_item_system.initialize( ) )
			{
				INIT_FAIL( "failed to initialize econ item system." );
			}
		}

		{
			// vac diagnostic hooks disabled: build is for -insecure/offline use only
			STAGE( "hooks::utility::initialize" );
			if ( !PROTECTION_CHECK( ) || !hooks::utility::initialize( ) )
			{
				INIT_FAIL( "failed to initialize utility hooks." );
			}

			STAGE( "hooks::cheat::initialize" );
			if ( !PROTECTION_CHECK( ) || !hooks::cheat::initialize( ) )
			{
				INIT_FAIL( "failed to initialize cheat hooks." );
			}
		}

		{
			STAGE( "PROTECTION_CHECK" );
			if ( !PROTECTION_CHECK( ) || !addresses::globals::cvar->unlock_all( ) )
			{
				INIT_FAIL( "failed to unlock hidden cvars." );
			}
		}

		STAGE( "features::world::g_scene.discover_skyboxes" );
		features::world::g_scene.discover_skyboxes( );
		return 1;
	}

} // namespace

extern "C" int __stdcall entry( HMODULE module_handle, DWORD reason, LPVOID reserved )
{
	if ( reason == DLL_PROCESS_ATTACH )
	{
		_CRT_INIT( module_handle, reason, reserved );
#if defined( DEV )
		AddVectoredExceptionHandler( 1, crash_logger );
		STAGE( "dll attach" );
#endif
		DisableThreadLibraryCalls( module_handle );

		const auto thread = CreateThread( nullptr, 0, init_thread, module_handle, 0, nullptr );
		if ( !thread )
		{
			return 0;
		}

		CloseHandle( thread );
		return 1;
	}
#if defined( DEV )
	else if ( reason == DLL_PROCESS_DETACH )
	{
		_CRT_INIT( module_handle, reason, reserved );

		features::world::g_weather.release( );
		rendering::g_menu.shutdown( );

		systems::events::shutdown( );
		hooks::utility::shutdown( );
		hooks::cheat::shutdown( );
		CoUninitialize( );
	}
#endif

	return 1;
}
