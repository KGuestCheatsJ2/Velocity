#include <pch/pch.hpp>
#include <d3d11.h>
#include <dxgi.h>
#include <utilities/memory/memory.hpp>
#include <utilities/logging/logging.hpp>
#include <protection/game_addresses.hpp>
#include "../addresses.hpp"

namespace addresses::functions {

	namespace {

		// Reads the DXGI swap chain vtable from a temporary, hidden device. Every IDXGISwapChain created by the
		// system shares the same implementation, so the Present / ResizeBuffers addresses are identical to the
		// ones used by the game. This needs no game pattern, so it survives game updates.
		bool resolve_swap_chain_functions( )
		{
			using create_fn = HRESULT( WINAPI* )( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext** );

			const auto d3d11 = GetModuleHandleA( "d3d11.dll" );
			if ( !d3d11 )
			{
				logging::console::print( "[functions] d3d11.dll is not loaded" );
				return false;
			}

			const auto create = reinterpret_cast< create_fn >( GetProcAddress( d3d11, "D3D11CreateDeviceAndSwapChain" ) );
			if ( !create )
			{
				logging::console::print( "[functions] D3D11CreateDeviceAndSwapChain not found" );
				return false;
			}

			WNDCLASSEXA wc{};
			wc.cbSize = sizeof( wc );
			wc.style = CS_CLASSDC;
			wc.lpfnWndProc = DefWindowProcA;
			wc.hInstance = GetModuleHandleA( nullptr );
			wc.lpszClassName = "cw_dummy_window";
			RegisterClassExA( &wc );

			const auto hwnd = CreateWindowExA( 0, wc.lpszClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr );
			if ( !hwnd )
			{
				UnregisterClassA( wc.lpszClassName, wc.hInstance );
				logging::console::print( "[functions] failed to create the temporary window" );
				return false;
			}

			DXGI_SWAP_CHAIN_DESC desc{};
			desc.BufferCount = 1;
			desc.BufferDesc.Width = 100;
			desc.BufferDesc.Height = 100;
			desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.OutputWindow = hwnd;
			desc.SampleDesc.Count = 1;
			desc.Windowed = TRUE;
			desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL level{};
			ID3D11Device* device{};
			ID3D11DeviceContext* context{};
			IDXGISwapChain* swap_chain{};

			auto hr = create( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &desc, &swap_chain, &device, &level, &context );
			if ( FAILED( hr ) )
			{
				hr = create( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &desc, &swap_chain, &device, &level, &context );
			}

			auto ok = false;
			if ( SUCCEEDED( hr ) && swap_chain )
			{
				const auto vtable = *reinterpret_cast< void*** >( swap_chain );
				present = reinterpret_cast< std::uintptr_t >( vtable[ 8 ] );
				resize_buffers = reinterpret_cast< std::uintptr_t >( vtable[ 13 ] );
				ok = present && resize_buffers;
			}
			else
			{
				logging::console::print( "[functions] failed to create the temporary swap chain (hr={:#x})", static_cast< std::uint32_t >( hr ) );
			}

			if ( swap_chain )
			{
				swap_chain->Release( );
			}
			if ( context )
			{
				context->Release( );
			}
			if ( device )
			{
				device->Release( );
			}

			DestroyWindow( hwnd );
			UnregisterClassA( wc.lpszClassName, wc.hInstance );

			return ok;
		}

	} // namespace

	bool initialize( )
	{
		if ( !resolve_swap_chain_functions( ) )
		{
			return false;
		}

		logging::console::print( "[functions] present: {:#x}, resize_buffers: {:#x}", present, resize_buffers );
		return true;
	}

} // namespace addresses::functions
