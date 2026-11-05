#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>

#include "../world.hpp"

// was a deadend.

namespace features::world {

	void smoke::on_map( std::uintptr_t token, std::size_t size, std::uintptr_t buf_ptr )
	{
		if ( size != 2480 || !token || !buf_ptr )
		{
			return;
		}

		this->m_token = token;
		this->m_buf = buf_ptr;
	}

	void smoke::on_unmap( std::uintptr_t token )
	{
		const auto buf = this->m_buf;
		if ( !buf || token != this->m_token )
		{
			return;
		}

		this->m_buf = 0;
		this->m_token = 0;
	}

} // namespace features::world