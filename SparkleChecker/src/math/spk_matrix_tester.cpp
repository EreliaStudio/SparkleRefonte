#include "math/spk_matrix_tester.hpp"

#include "math/spk_vector3.hpp"

TEST_F(MatrixTest, MatrixConstructor)
{
	spk::Matrix4x4 defaultMatrix;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			if (i == j)
				ASSERT_EQ(defaultMatrix[i][j], 1) << "Default matrix on [" << i << "][" << j << "] should be 1";
			else
				ASSERT_EQ(defaultMatrix[i][j], 0) << "Default matrix on [" << i << "][" << j << "] should be 0";
		}
	}
}

TEST_F(MatrixTest, MatrixConstructorOrder)
{
	float data[16] = {
			2, 0, 0, 3,
			0, 2, 0, 4,
			0, 0, 2, 5,
			0, 0, 0, 1
	};

	spk::Matrix4x4 customMatrix = spk::Matrix4x4(data);

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			ASSERT_EQ(customMatrix[i][j], data[i * 4 + j]) << "Matrix should be column major";
		}
	}
}

TEST_F(MatrixTest, MatrixOnVector3Application)
{
	spk::Matrix4x4 customMatrix = spk::Matrix4x4({
			2, 0, 0, 3,
			0, 2, 0, 4,
			0, 0, 2, 5,
			0, 0, 0, 1
		});

	spk::Vector3 position = spk::Vector3(1, 2, 3);

	spk::Vector3 computedPosition = customMatrix * position;

	ASSERT_EQ(computedPosition, spk::Vector3(5, 8, 11)) << "Wrong matrix multiplication";
}

TEST_F(MatrixTest, MatrixOnVector2Application)
{
	spk::Matrix3x3 customMatrix = spk::Matrix3x3({
		2, 0, 3,
		0, 2, 4,
		0, 0, 1
		});

	spk::Vector2 position = spk::Vector2(1, 2);

	spk::Vector2 computedPosition = customMatrix * position;

	ASSERT_EQ(computedPosition, spk::Vector2(5, 8)) << "Wrong matrix multiplication";
}

TEST_F(MatrixTest, MatrixComparator)
{
	spk::Matrix4x4 matrix = spk::Matrix4x4({
			2, 0, 0, 3,
			0, 2, 0, 4,
			0, 0, 2, 5,
			0, 0, 0, 1
		});

	spk::Matrix4x4 differentMatrix = spk::Matrix4x4({
			2, 0, 0, 3,
			0, 2, 0, 5,
			0, 0, 2, 5,
			0, 0, 0, 1
		});

	spk::Matrix4x4 identicalMatrix = spk::Matrix4x4({
			2, 0, 0, 3,
			0, 2, 0, 4,
			0, 0, 2, 5,
			0, 0, 0, 1
		});

	EXPECT_NE(matrix, differentMatrix);
	ASSERT_EQ(matrix, identicalMatrix);
}

TEST_F(MatrixTest, MatrixToString)
{
	spk::Matrix4x4 matrix = spk::Matrix4x4({
		2, 0, 0, 3,
		0, 2, 0, 4,
		0, 0, 2, 5,
		0, 0, 0, 1
		});

	std::wstring expected =
		L"[2 - 0 - 0 - 3] - "
		L"[0 - 2 - 0 - 4] - "
		L"[0 - 0 - 2 - 5] - "
		L"[0 - 0 - 0 - 1]";

	ASSERT_EQ(matrix.to_string(), expected);
}

TEST_F(MatrixTest, MatrixOutputStream_wostream)
{
	spk::Matrix4x4 matrix = spk::Matrix4x4({
		2, 0, 0, 3,
		0, 2, 0, 4,
		0, 0, 2, 5,
		0, 0, 0, 1
		});

	std::wostringstream woss;
	woss << matrix;
	std::wstring expected =
		L"[2 - 0 - 0 - 3] - "
		L"[0 - 2 - 0 - 4] - "
		L"[0 - 0 - 2 - 5] - "
		L"[0 - 0 - 0 - 1]";

	ASSERT_EQ(woss.str(), expected);
}

TEST_F(MatrixTest, MatrixOutputStream_ostream)
{
	spk::Matrix4x4 matrix = spk::Matrix4x4({
		2, 0, 0, 3,
		0, 2, 0, 4,
		0, 0, 2, 5,
		0, 0, 0, 1
		});

	std::ostringstream oss;
	oss << matrix;
	std::wstring expected_wstr =
		L"[2 - 0 - 0 - 3] - "
		L"[0 - 2 - 0 - 4] - "
		L"[0 - 0 - 2 - 5] - "
		L"[0 - 0 - 0 - 1]";
	std::string expected(expected_wstr.begin(), expected_wstr.end());

	ASSERT_EQ(oss.str(), expected);
}

TEST_F(MatrixTest, RotationMatrix)
{
	spk::Vector3 position = spk::Vector3(1, 0, 0);

	ASSERT_EQ(spk::Matrix4x4::rotationMatrix(90, 0, 0) * position, spk::Vector3(1, 0, 0)) << "Wrong matrix multiplication";

	ASSERT_EQ(spk::Matrix4x4::rotationMatrix(0, 90, 0) * position, spk::Vector3(0, 0, -1)) << "Wrong matrix multiplication";

	ASSERT_EQ(spk::Matrix4x4::rotationMatrix(0, 0, 90) * position, spk::Vector3(0, 1, 0)) << "Wrong matrix multiplication";
}

TEST_F(MatrixTest, TranslationMatrix)
{
	spk::Vector3 position = spk::Vector3(1, 0, 0);

	spk::Vector3 translatedPosition = spk::Matrix4x4::translationMatrix(1, 2, 3) * position;

	ASSERT_EQ(translatedPosition, spk::Vector3(2, 2, 3)) << "Wrong matrix multiplication for translation";

	translatedPosition = spk::Matrix4x4::translationMatrix(0, 0, 0) * position;

	ASSERT_EQ(translatedPosition, position) << "Wrong matrix multiplication for identity translation";
}

TEST_F(MatrixTest, ScaleMatrix)
{
	spk::Vector3 position = spk::Vector3(1, 1, 1);

	spk::Vector3 scaledPosition = spk::Matrix4x4::scaleMatrix(2, 3, 4) * position;

	ASSERT_EQ(scaledPosition, spk::Vector3(2, 3, 4)) << "Wrong matrix multiplication for scaling";

	scaledPosition = spk::Matrix4x4::scaleMatrix(1, 1, 1) * position;

	ASSERT_EQ(scaledPosition, position) << "Wrong matrix multiplication for identity scaling";
}
