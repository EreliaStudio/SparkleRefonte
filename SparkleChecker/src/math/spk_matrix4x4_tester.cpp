#include "math/spk_matrix4x4_tester.hpp"

TEST_F(Matrix4x4Test, DefaultConstructor)
{
	spk::Matrix4x4 matrix;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (i == j)
			{
				ASSERT_EQ(matrix.data[i][j], 1.0f) << "Diagonal elements should be 1 in the identity matrix";
			}
			else
			{
				ASSERT_EQ(matrix.data[i][j], 0.0f) << "Non-diagonal elements should be 0 in the identity matrix";
			}
		}
	}
}

TEST_F(Matrix4x4Test, CustomConstructor)
{
	ASSERT_EQ(customMatrix.data[0][0], 1);
	ASSERT_EQ(customMatrix.data[0][1], 2);
	ASSERT_EQ(customMatrix.data[0][2], 3);
	ASSERT_EQ(customMatrix.data[0][3], 4);
	ASSERT_EQ(customMatrix.data[1][0], 5);
	ASSERT_EQ(customMatrix.data[1][1], 6);
	ASSERT_EQ(customMatrix.data[1][2], 7);
	ASSERT_EQ(customMatrix.data[1][3], 8);
	ASSERT_EQ(customMatrix.data[2][0], 9);
	ASSERT_EQ(customMatrix.data[2][1], 10);
	ASSERT_EQ(customMatrix.data[2][2], 11);
	ASSERT_EQ(customMatrix.data[2][3], 12);
	ASSERT_EQ(customMatrix.data[3][0], 13);
	ASSERT_EQ(customMatrix.data[3][1], 14);
	ASSERT_EQ(customMatrix.data[3][2], 15);
	ASSERT_EQ(customMatrix.data[3][3], 16);
}

TEST_F(Matrix4x4Test, MatrixAddition)
{
	spk::Matrix4x4 result = identityMatrix + customMatrix;
	ASSERT_EQ(result.data[0][0], 2);
	ASSERT_EQ(result.data[0][1], 2);
	ASSERT_EQ(result.data[0][2], 3);
	ASSERT_EQ(result.data[0][3], 4);
	ASSERT_EQ(result.data[1][0], 5);
	ASSERT_EQ(result.data[1][1], 7);
	ASSERT_EQ(result.data[1][2], 7);
	ASSERT_EQ(result.data[1][3], 8);
	ASSERT_EQ(result.data[2][0], 9);
	ASSERT_EQ(result.data[2][1], 10);
	ASSERT_EQ(result.data[2][2], 12);
	ASSERT_EQ(result.data[2][3], 12);
	ASSERT_EQ(result.data[3][0], 13);
	ASSERT_EQ(result.data[3][1], 14);
	ASSERT_EQ(result.data[3][2], 15);
	ASSERT_EQ(result.data[3][3], 17);
}

TEST_F(Matrix4x4Test, MatrixMultiplication)
{
	spk::Matrix4x4 result = identityMatrix * customMatrix;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ASSERT_EQ(result.data[i][j], customMatrix.data[i][j]);
		}
	}
}

TEST_F(Matrix4x4Test, Vector3Multiplication)
{
	spk::Vector3 vec(1, 2, 3);
	spk::Vector3 result = customMatrix * vec;
	ASSERT_EQ(result.x, 18);
	ASSERT_EQ(result.y, 46);
	ASSERT_EQ(result.z, 74);
}

TEST_F(Matrix4x4Test, Vector2Multiplication)
{
	spk::Vector2 vec(1, 2);
	spk::Vector2 result = customMatrix * vec;
	ASSERT_EQ(result.x, 5);
	ASSERT_EQ(result.y, 11);
}

TEST_F(Matrix4x4Test, Transpose)
{
	spk::Matrix4x4 result = customMatrix.transpose();
	ASSERT_EQ(result.data[0][0], 1);
	ASSERT_EQ(result.data[1][0], 2);
	ASSERT_EQ(result.data[2][0], 3);
	ASSERT_EQ(result.data[3][0], 4);
	ASSERT_EQ(result.data[0][1], 5);
	ASSERT_EQ(result.data[1][1], 6);
	ASSERT_EQ(result.data[2][1], 7);
	ASSERT_EQ(result.data[3][1], 8);
	ASSERT_EQ(result.data[0][2], 9);
	ASSERT_EQ(result.data[1][2], 10);
	ASSERT_EQ(result.data[2][2], 11);
	ASSERT_EQ(result.data[3][2], 12);
	ASSERT_EQ(result.data[0][3], 13);
	ASSERT_EQ(result.data[1][3], 14);
	ASSERT_EQ(result.data[2][3], 15);
	ASSERT_EQ(result.data[3][3], 16);
}

TEST_F(Matrix4x4Test, Determinant2x2)
{
	float determinant = identityMatrix.determinant2x2(1, 2, 3, 4);
	ASSERT_EQ(determinant, -2.0f);
}

TEST_F(Matrix4x4Test, Inverse)
{
	spk::Matrix4x4 matrix(
		4, 7, 2, 0,
		3, 6, 1, 0,
		2, 5, 1, 0,
		0, 0, 0, 1);

	spk::Matrix4x4 expectedInverse(
		1, -1, 1, 0,
		-2, 2, -1, 0,
		1.5, -1.5, 0.5, 0,
		0, 0, 0, 1);

	spk::Matrix4x4 result = matrix.inverse();
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ASSERT_NEAR(result.data[i][j], expectedInverse.data[i][j], 1e-6);
		}
	}
}

TEST_F(Matrix4x4Test, Equality)
{
	spk::Matrix4x4 sameAsCustom(
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16);

	ASSERT_TRUE(customMatrix == sameAsCustom);
	ASSERT_FALSE(customMatrix == identityMatrix);
}

TEST_F(Matrix4x4Test, Inequality)
{
	spk::Matrix4x4 differentMatrix(
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 13,
		13, 14, 15, 16);

	ASSERT_TRUE(customMatrix != differentMatrix);
	ASSERT_FALSE(customMatrix != customMatrix);
}

TEST_F(Matrix4x4Test, TranslationMatrix)
{
	spk::Vector3 translation(2, 3, 4);
	spk::Matrix4x4 result = spk::Matrix4x4::translationMatrix(translation);

	ASSERT_EQ(result.data[0][3], 2);
	ASSERT_EQ(result.data[1][3], 3);
	ASSERT_EQ(result.data[2][3], 4);
	ASSERT_EQ(result.data[3][3], 1);
}

TEST_F(Matrix4x4Test, ScaleMatrix)
{
	spk::Vector3 scale(2, 3, 4);
	spk::Matrix4x4 result = spk::Matrix4x4::scaleMatrix(scale);

	ASSERT_EQ(result.data[0][0], 2);
	ASSERT_EQ(result.data[1][1], 3);
	ASSERT_EQ(result.data[2][2], 4);
	ASSERT_EQ(result.data[3][3], 1);
}

TEST_F(Matrix4x4Test, RotateAroundAxis)
{
	spk::Vector3 axis(0, 1, 0);
	float angle = static_cast<float>(M_PI / 4.0f); // 45 degrees

	spk::Matrix4x4 result = spk::Matrix4x4::rotateAroundAxis(axis, angle);

	ASSERT_NEAR(result.data[0][0], cos(angle), 1e-6);
	ASSERT_NEAR(result.data[0][2], sin(angle), 1e-6);
	ASSERT_NEAR(result.data[2][0], -sin(angle), 1e-6);
	ASSERT_NEAR(result.data[2][2], cos(angle), 1e-6);
	ASSERT_EQ(result.data[1][1], 1);
}

TEST_F(Matrix4x4Test, Perspective)
{
	float fov = static_cast<float>(M_PI / 4.0f); // 45 degrees
	float aspectRatio = 1.0f;
	float nearPlane = 1.0f;
	float farPlane = 10.0f;

	spk::Matrix4x4 result = spk::Matrix4x4::perspective(fov, aspectRatio, nearPlane, farPlane);

	ASSERT_NEAR(result.data[0][0], 1 / tan(fov / 2), 1e-6);
	ASSERT_NEAR(result.data[1][1], 1 / tan(fov / 2), 1e-6);
	ASSERT_NEAR(result.data[2][2], -(farPlane + nearPlane) / (farPlane - nearPlane), 1e-6);
	ASSERT_NEAR(result.data[2][3], -(2 * farPlane * nearPlane) / (farPlane - nearPlane), 1e-6);
	ASSERT_EQ(result.data[3][2], -1);
	ASSERT_EQ(result.data[3][3], 0);
}

TEST_F(Matrix4x4Test, Ortho)
{
	float left = -1.0f;
	float right = 1.0f;
	float bottom = -1.0f;
	float top = 1.0f;
	float nearPlane = -1.0f;
	float farPlane = 1.0f;

	spk::Matrix4x4 result = spk::Matrix4x4::ortho(left, right, bottom, top, nearPlane, farPlane);

	ASSERT_EQ(result.data[0][0], 2 / (right - left));
	ASSERT_EQ(result.data[1][1], 2 / (top - bottom));
	ASSERT_EQ(result.data[2][2], -2 / (farPlane - nearPlane));
	ASSERT_EQ(result.data[0][3], -(right + left) / (right - left));
	ASSERT_EQ(result.data[1][3], -(top + bottom) / (top - bottom));
	ASSERT_EQ(result.data[2][3], -(farPlane + nearPlane) / (farPlane - nearPlane));
}