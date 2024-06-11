#include "structure/math/spk_vector2_tester.hpp"

TEST_F(Vector2Test, DefaultConstructor)
{
	spk::IVector2<int> vecInt;
	ASSERT_EQ(vecInt.x, 0);
	ASSERT_EQ(vecInt.y, 0);

	spk::IVector2<float> vecFloat;
	ASSERT_EQ(vecFloat.x, 0.0f);
	ASSERT_EQ(vecFloat.y, 0.0f);

	spk::IVector2<double> vecDouble;
	ASSERT_EQ(vecDouble.x, 0.0);
	ASSERT_EQ(vecDouble.y, 0.0);

	spk::IVector2<size_t> vecSizeT;
	ASSERT_EQ(vecSizeT.x, 0);
	ASSERT_EQ(vecSizeT.y, 0);
}

TEST_F(Vector2Test, ScalarConstructor)
{
	spk::IVector2<int> vecInt(5);
	ASSERT_EQ(vecInt.x, 5);
	ASSERT_EQ(vecInt.y, 5);

	spk::IVector2<float> vecFloat(5.5f);
	ASSERT_FLOAT_EQ(vecFloat.x, 5.5f);
	ASSERT_FLOAT_EQ(vecFloat.y, 5.5f);

	spk::IVector2<double> vecDouble(5.5);
	ASSERT_DOUBLE_EQ(vecDouble.x, 5.5);
	ASSERT_DOUBLE_EQ(vecDouble.y, 5.5);

	spk::IVector2<size_t> vecSizeT(5);
	ASSERT_EQ(vecSizeT.x, 5);
	ASSERT_EQ(vecSizeT.y, 5);
}

TEST_F(Vector2Test, TwoValueConstructor)
{
	spk::IVector2<int> vecInt(3, 4);
	ASSERT_EQ(vecInt.x, 3);
	ASSERT_EQ(vecInt.y, 4);

	spk::IVector2<float> vecFloat(3.5f, 4.5f);
	ASSERT_FLOAT_EQ(vecFloat.x, 3.5f);
	ASSERT_FLOAT_EQ(vecFloat.y, 4.5f);

	spk::IVector2<double> vecDouble(3.5, 4.5);
	ASSERT_DOUBLE_EQ(vecDouble.x, 3.5);
	ASSERT_DOUBLE_EQ(vecDouble.y, 4.5);

	spk::IVector2<size_t> vecSizeT(3, 4);
	ASSERT_EQ(vecSizeT.x, 3);
	ASSERT_EQ(vecSizeT.y, 4);
}

TEST_F(Vector2Test, CopyConstructor)
{
	spk::IVector2<int> vecInt1(3, 4);
	spk::IVector2<int> vecInt2(vecInt1);
	ASSERT_EQ(vecInt2.x, 3);
	ASSERT_EQ(vecInt2.y, 4);

	spk::IVector2<float> vecFloat1(3.5f, 4.5f);
	spk::IVector2<float> vecFloat2(vecFloat1);
	ASSERT_FLOAT_EQ(vecFloat2.x, 3.5f);
	ASSERT_FLOAT_EQ(vecFloat2.y, 4.5f);

	spk::IVector2<double> vecDouble1(3.5, 4.5);
	spk::IVector2<double> vecDouble2(vecDouble1);
	ASSERT_DOUBLE_EQ(vecDouble2.x, 3.5);
	ASSERT_DOUBLE_EQ(vecDouble2.y, 4.5);

	spk::IVector2<size_t> vecSizeT1(3, 4);
	spk::IVector2<size_t> vecSizeT2(vecSizeT1);
	ASSERT_EQ(vecSizeT2.x, 3);
	ASSERT_EQ(vecSizeT2.y, 4);
}

TEST_F(Vector2Test, EqualityOperator)
{
	spk::IVector2<int> vecInt1(3, 4);
	spk::IVector2<int> vecInt2(3, 4);
	spk::IVector2<int> vecInt3(5, 6);
	ASSERT_EQ(vecInt1, vecInt2);
	ASSERT_NE(vecInt1, vecInt3);

	spk::IVector2<float> vecFloat1(3.0f, 4.0f);
	spk::IVector2<float> vecFloat2(3.0f, 4.0f);
	spk::IVector2<float> vecFloat3(5.0f, 6.0f);
	ASSERT_EQ(vecFloat1, vecFloat2);
	ASSERT_NE(vecFloat1, vecFloat3);

	spk::IVector2<double> vecDouble1(3.0, 4.0);
	spk::IVector2<double> vecDouble2(3.0, 4.0);
	spk::IVector2<double> vecDouble3(5.0, 6.0);
	ASSERT_EQ(vecDouble1, vecDouble2);
	ASSERT_NE(vecDouble1, vecDouble3);

	spk::IVector2<size_t> vecSizeT1(3, 4);
	spk::IVector2<size_t> vecSizeT2(3, 4);
	spk::IVector2<size_t> vecSizeT3(5, 6);
	ASSERT_EQ(vecSizeT1, vecSizeT2);
	ASSERT_NE(vecSizeT1, vecSizeT3);


	spk::IVector2<int> vecInt4(3, 4);
	spk::IVector2<float> vecFloat4(3.5f, 4.5f);

	ASSERT_TRUE(vecInt4 == vecFloat4) << "Casting the IVector2<float> into IVector2<int> didn't make it lose it's tail";
	ASSERT_FALSE(vecFloat4 == vecInt4) << "Casting the IVector2<int> into IVector2<float> shouldn't make the equality true";
}

TEST_F(Vector2Test, MixedTypeEqualityOperator)
{
	spk::IVector2<int> vecInt(3, 4);
	spk::IVector2<float> vecFloat(3.0f, 4.0f);
	spk::IVector2<double> vecDouble(3.0, 4.0);
	spk::IVector2<size_t> vecSizeT(3, 4);

	ASSERT_EQ(vecInt, vecFloat);
	ASSERT_EQ(vecInt, vecDouble);
	ASSERT_EQ(vecInt, vecSizeT);
	ASSERT_EQ(vecFloat, vecDouble);
	ASSERT_EQ(vecFloat, vecSizeT);
	ASSERT_EQ(vecDouble, vecSizeT);

	spk::IVector2<int> vecIntDifferent(5, 6);
	spk::IVector2<float> vecFloatDifferent(5.0f, 6.0f);
	spk::IVector2<double> vecDoubleDifferent(5.0, 6.0);
	spk::IVector2<size_t> vecSizeTDifferent(5, 6);

	ASSERT_NE(vecInt, vecIntDifferent);
	ASSERT_NE(vecInt, vecFloatDifferent);
	ASSERT_NE(vecInt, vecDoubleDifferent);
	ASSERT_NE(vecInt, vecSizeTDifferent);
	ASSERT_NE(vecFloat, vecFloatDifferent);
	ASSERT_NE(vecFloat, vecDoubleDifferent);
	ASSERT_NE(vecFloat, vecSizeTDifferent);
	ASSERT_NE(vecDouble, vecDoubleDifferent);
	ASSERT_NE(vecDouble, vecSizeTDifferent);
	ASSERT_NE(vecSizeT, vecSizeTDifferent);
}

TEST_F(Vector2Test, ToStringMethod)
{
	ASSERT_EQ(spk::IVector2<int>(3, 4).to_string(), L"(3, 4)");

	ASSERT_EQ(spk::IVector2<float>(3.0f, 4.0f).to_string(), L"(3, 4)");

	ASSERT_EQ(spk::IVector2<double>(3.0, 4.0).to_string(), L"(3, 4)");

	ASSERT_EQ(spk::IVector2<size_t>(3, 4).to_string(), L"(3, 4)");
}