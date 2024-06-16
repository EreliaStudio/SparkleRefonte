#include "math/spk_quaternion_tester.hpp"

TEST_F(QuaternionTest, DefaultConstructor)
{
	spk::IQuaternion<float> quat;
	ASSERT_EQ(quat, spk::IQuaternion<float>(1, 0, 0, 0));
}

TEST_F(QuaternionTest, ParameterizedConstructor)
{
	spk::IQuaternion<float> quat(1, 2, 3, 4);
	ASSERT_EQ(quat, spk::IQuaternion<float>(1, 2, 3, 4));
}

TEST_F(QuaternionTest, CopyConstructor)
{
	spk::IQuaternion<float> quat1(1, 0, 1, 0);
	spk::IQuaternion<float> quat_copy(quat1);
	ASSERT_EQ(quat_copy, quat1);
}

TEST_F(QuaternionTest, AssignmentOperator)
{
	spk::IQuaternion<float> quat1(1, 0.5, 0.5, 0.75);
	spk::IQuaternion<float> quat;
	quat = quat1;
	ASSERT_EQ(quat, quat1);
}

TEST_F(QuaternionTest, Normalize)
{
	spk::IQuaternion<float> quat(1, 0, 1, 0);
	auto quat_norm = quat.normalize();
	float len = std::sqrt(quat_norm.w * quat_norm.w + quat_norm.x * quat_norm.x + quat_norm.y * quat_norm.y + quat_norm.z * quat_norm.z);
	ASSERT_NEAR(len, 1.0, 1e-6);
}

TEST_F(QuaternionTest, MultiplyQuaternion)
{
	spk::IQuaternion<float> quat1(0, 1, 0, 1);
	spk::IQuaternion<float> quat2(0.75, 1, 0.5, 0.5);
	auto result = quat1 * quat2;
	spk::IQuaternion<float> expected(-1.5, 0.25, 0.5, 1.25);
	ASSERT_EQ(result, expected);
}

TEST_F(QuaternionTest, MultiplyVector)
{
	spk::IQuaternion<float> quat(0, 1, 0, 1);

	auto result = quat * spk::IVector3<float>(3.414, 42, 128);
	spk::IVector3<float> expected = spk::Vector3(128, -42, 3.414f).normalize();
	ASSERT_EQ(result, expected);

	result = quat * spk::IVector3<float>(3, 2, 1);
	expected = spk::Vector3(1, -2, 3).normalize();
	ASSERT_EQ(result, expected);

	spk::IQuaternion<float> quat90Y(0, std::sin(M_PI / 4), 0, std::cos(M_PI / 4));
	result = quat90Y * spk::IVector3<float>(1, 0, 0);
	expected = spk::Vector3(0, 0, -1).normalize();
	EXPECT_EQ(result, expected);

	spk::IQuaternion<float> quat90Z(0, 0, std::sin(M_PI / 4), std::cos(M_PI / 4));
	result = quat90Z * spk::IVector3<float>(0, 1, 0);
	expected = spk::Vector3(-1, 0, 0).normalize();
	EXPECT_EQ(result, expected);

	spk::IQuaternion<float> quat180X(std::sin(M_PI / 2), 0, 0, std::cos(M_PI / 2));
	result = quat180X * spk::IVector3<float>(0, 0, 1);
	expected = spk::Vector3(0, 0, -1).normalize();
	EXPECT_EQ(result, expected);

	spk::IQuaternion<float> quat120Arb(std::sin(2 * M_PI / 3 / 2) / std::sqrt(3), std::sin(2 * M_PI / 3 / 2) / std::sqrt(3), std::sin(2 * M_PI / 3 / 2) / std::sqrt(3), std::cos(2 * M_PI / 3 / 2));
	result = quat120Arb * spk::IVector3<float>(1, 2, 3);
	expected = spk::Vector3(-2.23205, -1.23205, 1.4641).normalize();
	EXPECT_EQ(result, expected);

	spk::IQuaternion<float> quatIdentity(0, 0, 0, 1);
	result = quatIdentity * spk::IVector3<float>(5, -3, 2);
	expected = spk::Vector3(5, -3, 2).normalize();
	EXPECT_EQ(result, expected);
}

TEST_F(QuaternionTest, RotationFromEuler)
{
	auto quat_rot = spk::IQuaternion<float>::rotationFromEuler(20, 40, 60);
	spk::IQuaternion<float> expected(0.831130, -0.0270976, 0.373286, 0.411274);
	EXPECT_EQ(quat_rot, expected);

	spk::Vector3 point = spk::Vector3(1, 0, 0);
	spk::Vector3 expectedPoint = spk::Vector3(0.7198, 0.3521, 0.5982);
	EXPECT_EQ(quat_rot * point, expectedPoint);
}

TEST_F(QuaternionTest, RotateAroundAxis)
{
	spk::IVector3<float> axis(1, 0, 0);
	auto quat_rot = spk::IQuaternion<float>::rotateAroundAxis(axis, 90);
	spk::IQuaternion<float> expected(0.7071068, 0.7071068, 0, 0);
	ASSERT_EQ(quat_rot, expected);
}

TEST_F(QuaternionTest, LookAt)
{
	spk::IVector3<float> source(0, 0, 0);
	spk::IVector3<float> destination(1, 0, 0);
	spk::IVector3<float> up(0, 1, 0);
	auto quat_lookat = spk::IQuaternion<float>::lookAt(source, destination, up);
	spk::IQuaternion<float> expected(0.7071068, 0, 0, 0.7071068);
	ASSERT_EQ(quat_lookat, expected);
}

TEST_F(QuaternionTest, ToMatrix)
{
	spk::IQuaternion<float> quat(1, 0, 1, 0);
	auto matrix = quat.toMatrix();
	spk::IMatrix<4, 4> expected_matrix{
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	ASSERT_EQ(matrix, expected_matrix);
}
