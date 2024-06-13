#pragma once

#include "gtest/gtest.h"
#include "math/spk_matrix4x4.hpp"

using namespace spk;

class Matrix4x4Test : public ::testing::Test
{
protected:
	Matrix4x4 identityMatrix;
	Matrix4x4 customMatrix;

	void SetUp() override
	{
		identityMatrix = Matrix4x4();
		customMatrix = Matrix4x4(
			1, 2, 3, 4,
			5, 6, 7, 8,
			9, 10, 11, 12,
			13, 14, 15, 16);
	}
};
