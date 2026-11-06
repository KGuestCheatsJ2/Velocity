#include <pch/pch.hpp>
#include <utilities/logging/logging.hpp>

#include "../hooking.hpp"

namespace hooking::manager {

	bool create( const std::initializer_list<entry>& entries )
	{
		for ( const auto& entry : entries )
		{
			if ( !entry.address )
			{
				// pattern went stale after a game update: skip this hook, the rest keeps working
				logging::console::print( xs( "skipping hook (address not found): {}" ), entry.name );
				continue;
			}

			if ( !entry.hook->create( reinterpret_cast< void* >( entry.address ), entry.detour ) )
			{
				logging::console::print( xs( "failed to hook: {}" ), entry.name );
				return false;
			}

			if ( !entry.hook->enable( ) )
			{
				return false;
			}

		}

		return true;
	}

} // namespace hooking::manager