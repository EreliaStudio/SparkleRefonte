#pragma once

#include "structure/math/spk_vector2.hpp"

namespace spk
{
	struct Controller
	{
		enum class Button
		{
			A,
			B,
			X,
			Y,
			LeftArrow,
			DownArrow,
			RightArrow,
			UpArrow,
			Start,
			Select,
			LeftStick,
			RightStick
		};

		struct Joystick
		{
			enum ID
			{
				Left,
				Right
			};
			spk::Vector2Int delta;
			spk::Vector2Int position;
		};

		Joystick leftJoystick;
		Joystick rightJoystick;
	};
}