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