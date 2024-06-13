#include "math/spk_quaternion_tester.hpp"

TEST_F(QuaternionTest, DefaultConstructor)
{
    spk::Quaternion quat;
    EXPECT_EQ(quat.x, 0) << "Default constructor should initialize x to 0";
    EXPECT_EQ(quat.y, 0) << "Default constructor should initialize y to 0";
    EXPECT_EQ(quat.z, 0) << "Default constructor should initialize z to 0";
    EXPECT_EQ(quat.w, 1) << "Default constructor should initialize w to 1";
}

TEST_F(QuaternionTest, ParametrizedConstructor)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(quat.x, 1.0f) << "Parametrized constructor should initialize x to 1.0f";
    EXPECT_EQ(quat.y, 2.0f) << "Parametrized constructor should initialize y to 2.0f";
    EXPECT_EQ(quat.z, 3.0f) << "Parametrized constructor should initialize z to 3.0f";
    EXPECT_EQ(quat.w, 4.0f) << "Parametrized constructor should initialize w to 4.0f";
}

TEST_F(QuaternionTest, EulerAnglesConstructor)
{
    spk::Vector3 eulerAngles(90.0f, 0.0f, 0.0f);
    spk::Quaternion quat(eulerAngles);
    spk::Quaternion expected = spk::Quaternion::fromAxisAngle(spk::Vector3(1, 0, 0), 90.0f);
    EXPECT_EQ(quat, expected) << "Euler angles constructor should correctly convert Euler angles to quaternion";
}

TEST_F(QuaternionTest, AdditionOperator)
{
    spk::Quaternion quat1(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Quaternion quat2(5.0f, 6.0f, 7.0f, 8.0f);
    spk::Quaternion result = quat1 + quat2;
    EXPECT_EQ(result, spk::Quaternion(6.0f, 8.0f, 10.0f, 12.0f)) << "Addition operator should add correctly";
}

TEST_F(QuaternionTest, SubtractionOperator)
{
    spk::Quaternion quat1(5.0f, 6.0f, 7.0f, 8.0f);
    spk::Quaternion quat2(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Quaternion result = quat1 - quat2;
    EXPECT_EQ(result, spk::Quaternion(4.0f, 4.0f, 4.0f, 4.0f)) << "Subtraction operator should subtract correctly";
}

TEST_F(QuaternionTest, MultiplicationByScalar)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    float scalar = 2.0f;
    spk::Quaternion result = quat * scalar;
    EXPECT_EQ(result, spk::Quaternion(2.0f, 4.0f, 6.0f, 8.0f)) << "Multiplication by scalar should multiply correctly";
}

TEST_F(QuaternionTest, DivisionByScalar)
{
    spk::Quaternion quat(2.0f, 4.0f, 6.0f, 8.0f);
    float scalar = 2.0f;
    spk::Quaternion result = quat / scalar;
    EXPECT_EQ(result, spk::Quaternion(1.0f, 2.0f, 3.0f, 4.0f)) << "Division by scalar should divide correctly";
}

TEST_F(QuaternionTest, Normalization)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Quaternion result = quat.normalize();
    float length = sqrt(1.0f + 4.0f + 9.0f + 16.0f);
    EXPECT_EQ(result, spk::Quaternion(1.0f / length, 2.0f / length, 3.0f / length, 4.0f / length)) << "Normalization should normalize correctly";
}

TEST_F(QuaternionTest, Conjugation)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Quaternion result = quat.conjugate();
    EXPECT_EQ(result, spk::Quaternion(-1.0f, -2.0f, -3.0f, 4.0f)) << "Conjugation should conjugate correctly";
}

TEST_F(QuaternionTest, Inversion)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Quaternion result = quat.inverse();
    float length = sqrt(1.0f + 4.0f + 9.0f + 16.0f);
    spk::Quaternion expected = quat.conjugate().normalize();
    EXPECT_EQ(result, expected) << "Inversion should invert correctly";
}

TEST_F(QuaternionTest, MultiplicationByQuaternion)
{
    spk::Quaternion quat1(1.0f, 0.0f, 0.0f, 0.0f);
    spk::Quaternion quat2(0.0f, 1.0f, 0.0f, 0.0f);
    spk::Quaternion result = quat1 * quat2;
    EXPECT_EQ(result, spk::Quaternion(0.0f, 0.0f, 1.0f, 0.0f)) << "Multiplication by quaternion should multiply correctly";
}

TEST_F(QuaternionTest, LookAt)
{
    spk::Vector3 direction(1.0f, 0.0f, 0.0f);
    spk::Vector3 up(0.0f, 1.0f, 0.0f);
    spk::Quaternion result = spk::Quaternion::lookAt(direction, up);
    spk::Quaternion expected = spk::Quaternion::fromAxisAngle(spk::Vector3(0.0f, 1.0f, 0.0f), 90.0f);
    EXPECT_EQ(result, expected) << "LookAt should generate correct quaternion";
}

TEST_F(QuaternionTest, FromEulerAngles)
{
    spk::Vector3 eulerAngles(90.0f, 0.0f, 0.0f);
    spk::Quaternion result = spk::Quaternion::fromEulerAngles(eulerAngles);
    spk::Quaternion expected = spk::Quaternion::fromAxisAngle(spk::Vector3(1, 0, 0), 90.0f);
    EXPECT_EQ(result, expected) << "FromEulerAngles should convert correctly";
}

TEST_F(QuaternionTest, FromAxisAngle)
{
    spk::Vector3 axis(1.0f, 0.0f, 0.0f);
    float angle = 90.0f;
    spk::Quaternion result = spk::Quaternion::fromAxisAngle(axis, angle);
    float halfAngle = spk::degreeToRadian(angle) * 0.5f;
    float s = sin(halfAngle);
    spk::Quaternion expected(axis.x * s, axis.y * s, axis.z * s, cos(halfAngle));
    EXPECT_EQ(result, expected) << "FromAxisAngle should convert correctly";
}

TEST_F(QuaternionTest, ApplyRotation)
{
    spk::Quaternion quat(1.0f, 0.0f, 0.0f, 0.0f);
    spk::Vector3 point(0.0f, 1.0f, 0.0f);
    spk::Vector3 result = quat.applyRotation(point);
    spk::Vector3 expected(0.0f, 0.0f, -1.0f);
    EXPECT_EQ(result, expected) << "ApplyRotation should rotate correctly";
}

TEST_F(QuaternionTest, ToVector3)
{
    spk::Quaternion quat(1.0f, 2.0f, 3.0f, 4.0f);
    spk::Vector3 result = quat.toVector3();
    spk::Vector3 expected(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(result, expected) << "ToVector3 should convert correctly";
}
