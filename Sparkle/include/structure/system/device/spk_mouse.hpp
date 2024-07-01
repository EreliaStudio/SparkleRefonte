#pragma once

#include "structure/math/spk_vector2.hpp"

namespace spk
{
	struct Mouse
	{
		enum class Button
		{
			Left,
			Middle,
			Right
		};

		Vector2Int position;
	};
}

std::ostream& operator << (std::ostream & p_os, const spk::Mouse::Button & p_button);
std::wostream& operator << (std::wostream & p_os, const spk::Mouse::Button & p_button);