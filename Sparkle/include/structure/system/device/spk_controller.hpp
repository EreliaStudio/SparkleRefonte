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
			R1,
			L1,
			R2,
			L2,
			R3,
			L3,
			Unknow
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

std::ostream& operator << (std::ostream& p_os, const spk::Controller::Button& p_button);
std::ostream& operator << (std::ostream& p_os, const spk::Controller::Joystick::ID& p_joystickID);

std::wostream& operator << (std::wostream& p_os, const spk::Controller::Button& p_button);
std::wostream& operator << (std::wostream& p_os, const spk::Controller::Joystick::ID& p_joystickID);