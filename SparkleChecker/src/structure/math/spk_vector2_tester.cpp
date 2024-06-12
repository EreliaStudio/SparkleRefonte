#include "structure/math/spk_vector2_tester.hpp"

TEST_F(Vector2Test, DefaultConstructor)
{
	spk::IVector2<int> vecInt;
	EXPECT_EQ(vecInt.x, 0) << "Default constructor for int should initialize x to 0";
	EXPECT_EQ(vecInt.y, 0) << "Default constructor for int should initialize y to 0";

	spk::IVector2<float> vecFloat;
	EXPECT_EQ(vecFloat.x, 0.0f) << "Default constructor for float should initialize x to 0.0f";
	EXPECT_EQ(vecFloat.y, 0.0f) << "Default constructor for float should initialize y to 0.0f";

	spk::IVector2<double> vecDouble;
	EXPECT_EQ(vecDouble.x, 0.0) << "Default constructor for double should initialize x to 0.0";
	EXPECT_EQ(vecDouble.y, 0.0) << "Default constructor for double should initialize y to 0.0";

	spk::IVector2<size_t> vecSizeT;
	EXPECT_EQ(vecSizeT.x, 0) << "Default constructor for size_t should initialize x to 0";
	EXPECT_EQ(vecSizeT.y, 0) << "Default constructor for size_t should initialize y to 0";
}

TEST_F(Vector2Test, ScalarConstructor)
{
	spk::IVector2<int> vecInt(5);
	EXPECT_EQ(vecInt.x, 5) << "Scalar constructor for int should initialize x to 5";
	EXPECT_EQ(vecInt.y, 5) << "Scalar constructor for int should initialize y to 5";

	spk::IVector2<float> vecFloat(5.5f);
	EXPECT_FLOAT_EQ(vecFloat.x, 5.5f) << "Scalar constructor for float should initialize x to 5.5f";
	EXPECT_FLOAT_EQ(vecFloat.y, 5.5f) << "Scalar constructor for float should initialize y to 5.5f";

	spk::IVector2<double> vecDouble(5.5);
	EXPECT_DOUBLE_EQ(vecDouble.x, 5.5) << "Scalar constructor for double should initialize x to 5.5";
	EXPECT_DOUBLE_EQ(vecDouble.y, 5.5) << "Scalar constructor for double should initialize y to 5.5";

	spk::IVector2<size_t> vecSizeT(5);
	EXPECT_EQ(vecSizeT.x, 5) << "Scalar constructor for size_t should initialize x to 5";
	EXPECT_EQ(vecSizeT.y, 5) << "Scalar constructor for size_t should initialize y to 5";
}

TEST_F(Vector2Test, TwoValueConstructor)
{
	spk::IVector2<int> vecInt(3, 4);
	EXPECT_EQ(vecInt.x, 3) << "Two-value constructor for int should initialize x to 3";
	EXPECT_EQ(vecInt.y, 4) << "Two-value constructor for int should initialize y to 4";

	spk::IVector2<float> vecFloat(3.5f, 4.5f);
	EXPECT_FLOAT_EQ(vecFloat.x, 3.5f) << "Two-value constructor for float should initialize x to 3.5f";
	EXPECT_FLOAT_EQ(vecFloat.y, 4.5f) << "Two-value constructor for float should initialize y to 4.5f";

	spk::IVector2<double> vecDouble(3.5, 4.5);
	EXPECT_DOUBLE_EQ(vecDouble.x, 3.5) << "Two-value constructor for double should initialize x to 3.5";
	EXPECT_DOUBLE_EQ(vecDouble.y, 4.5) << "Two-value constructor for double should initialize y to 4.5";

	spk::IVector2<size_t> vecSizeT(3, 4);
	EXPECT_EQ(vecSizeT.x, 3) << "Two-value constructor for size_t should initialize x to 3";
	EXPECT_EQ(vecSizeT.y, 4) << "Two-value constructor for size_t should initialize y to 4";
}

TEST_F(Vector2Test, CopyConstructor)
{
	spk::IVector2<int> vecInt1(3, 4);
	spk::IVector2<int> vecInt2(vecInt1);
	EXPECT_EQ(vecInt2.x, 3) << "Copy constructor for int should copy x value";
	EXPECT_EQ(vecInt2.y, 4) << "Copy constructor for int should copy y value";

	spk::IVector2<float> vecFloat1(3.5f, 4.5f);
	spk::IVector2<float> vecFloat2(vecFloat1);
	EXPECT_FLOAT_EQ(vecFloat2.x, 3.5f) << "Copy constructor for float should copy x value";
	EXPECT_FLOAT_EQ(vecFloat2.y, 4.5f) << "Copy constructor for float should copy y value";

	spk::IVector2<double> vecDouble1(3.5, 4.5);
	spk::IVector2<double> vecDouble2(vecDouble1);
	EXPECT_DOUBLE_EQ(vecDouble2.x, 3.5) << "Copy constructor for double should copy x value";
	EXPECT_DOUBLE_EQ(vecDouble2.y, 4.5) << "Copy constructor for double should copy y value";

	spk::IVector2<size_t> vecSizeT1(3, 4);
	spk::IVector2<size_t> vecSizeT2(vecSizeT1);
	EXPECT_EQ(vecSizeT2.x, 3) << "Copy constructor for size_t should copy x value";
	EXPECT_EQ(vecSizeT2.y, 4) << "Copy constructor for size_t should copy y value";
}

TEST_F(Vector2Test, EqualityOperator)
{
	spk::IVector2<int> vecInt1(3, 4);
	spk::IVector2<int> vecInt2(3, 4);
	spk::IVector2<int> vecInt3(5, 6);
	EXPECT_EQ(vecInt1, vecInt2) << "Equality operator should return true for equal int vectors";
	EXPECT_NE(vecInt1, vecInt3) << "Equality operator should return false for different int vectors";

	spk::IVector2<float> vecFloat1(3.0f, 4.0f);
	spk::IVector2<float> vecFloat2(3.0f, 4.0f);
	spk::IVector2<float> vecFloat3(5.0f, 6.0f);
	EXPECT_EQ(vecFloat1, vecFloat2) << "Equality operator should return true for equal float vectors";
	EXPECT_NE(vecFloat1, vecFloat3) << "Equality operator should return false for different float vectors";

	spk::IVector2<double> vecDouble1(3.0, 4.0);
	spk::IVector2<double> vecDouble2(3.0, 4.0);
	spk::IVector2<double> vecDouble3(5.0, 6.0);
	EXPECT_EQ(vecDouble1, vecDouble2) << "Equality operator should return true for equal double vectors";
	EXPECT_NE(vecDouble1, vecDouble3) << "Equality operator should return false for different double vectors";

	spk::IVector2<size_t> vecSizeT1(3, 4);
	spk::IVector2<size_t> vecSizeT2(3, 4);
	spk::IVector2<size_t> vecSizeT3(5, 6);
	EXPECT_EQ(vecSizeT1, vecSizeT2) << "Equality operator should return true for equal size_t vectors";
	EXPECT_NE(vecSizeT1, vecSizeT3) << "Equality operator should return false for different size_t vectors";

	spk::IVector2<int> vecInt4(3, 4);
	spk::IVector2<float> vecFloat4(3.0f, 4.0f);

	EXPECT_TRUE(vecInt4 == vecFloat4) << "Mixed-type equality operator should return true for equivalent int and float vectors";
	EXPECT_TRUE(vecFloat4 == vecInt4) << "Mixed-type equality operator should return true for equivalent float and int vectors";
}

TEST_F(Vector2Test, MixedTypeEqualityOperator)
{
	spk::IVector2<int> vecInt(3, 4);
	spk::IVector2<float> vecFloat(3.0f, 4.0f);
	spk::IVector2<double> vecDouble(3.0, 4.0);
	spk::IVector2<size_t> vecSizeT(3, 4);

	EXPECT_EQ(vecInt, vecFloat) << "Mixed-type equality operator should return true for equivalent int and float vectors";
	EXPECT_EQ(vecInt, vecDouble) << "Mixed-type equality operator should return true for equivalent int and double vectors";
	EXPECT_EQ(vecInt, vecSizeT) << "Mixed-type equality operator should return true for equivalent int and size_t vectors";
	EXPECT_EQ(vecFloat, vecDouble) << "Mixed-type equality operator should return true for equivalent float and double vectors";
	EXPECT_EQ(vecFloat, vecSizeT) << "Mixed-type equality operator should return true for equivalent float and size_t vectors";
	EXPECT_EQ(vecDouble, vecSizeT) << "Mixed-type equality operator should return true for equivalent double and size_t vectors";

	spk::IVector2<int> vecIntDifferent(5, 6);
	spk::IVector2<float> vecFloatDifferent(5.0f, 6.0f);
	spk::IVector2<double> vecDoubleDifferent(5.0, 6.0);
	spk::IVector2<size_t> vecSizeTDifferent(5, 6);

	EXPECT_NE(vecInt, vecIntDifferent) << "Mixed-type equality operator should return false for different int and int vectors";
	EXPECT_NE(vecInt, vecFloatDifferent) << "Mixed-type equality operator should return false for different int and float vectors";
	EXPECT_NE(vecInt, vecDoubleDifferent) << "Mixed-type equality operator should return false for different int and double vectors";
	EXPECT_NE(vecInt, vecSizeTDifferent) << "Mixed-type equality operator should return false for different int and size_t vectors";
	EXPECT_NE(vecFloat, vecFloatDifferent) << "Mixed-type equality operator should return false for different float and float vectors";
	EXPECT_NE(vecFloat, vecDoubleDifferent) << "Mixed-type equality operator should return false for different float and double vectors";
	EXPECT_NE(vecFloat, vecSizeTDifferent) << "Mixed-type equality operator should return false for different float and size_t vectors";
	EXPECT_NE(vecDouble, vecDoubleDifferent) << "Mixed-type equality operator should return false for different double and double vectors";
	EXPECT_NE(vecDouble, vecSizeTDifferent) << "Mixed-type equality operator should return false for different double and size_t vectors";
	EXPECT_NE(vecSizeT, vecSizeTDifferent) << "Mixed-type equality operator should return false for different size_t and size_t vectors";
}

TEST_F(Vector2Test, ComparatorOperators)
{
	spk::IVector2<int> value(5, 6);
	spk::IVector2<int> xEqualYEqual(5, 6);
	spk::IVector2<int> xEqualYBigger(5, 7);
	spk::IVector2<int> xEqualYLower(5, 5);
	spk::IVector2<int> xBiggerYEqual(6, 6);
	spk::IVector2<int> xLowerYEqual(4, 6);

	//EXPECT_TRUE(value < xEqualYEqual) << "Comparing with X Equal and Y Bigger with operator < should return false";
	//EXPECT_TRUE(value < xEqualYBigger) << "Comparing with X Equal and Y Bigger with operator < should return false";
	//EXPECT_TRUE(value < xEqualYLower) << "Comparing with X Equal and Y Bigger with operator < should return true";
	//EXPECT_TRUE(value < xBiggerYEqual) << "Comparing with X Equal and Y Bigger with operator < should return false";
	//EXPECT_TRUE(value < xLowerYEqual) << "Comparing with X Equal and Y Bigger with operator < should return true";

	//EXPECT_TRUE(value > xEqualYEqual) << "Comparing with X Equal and Y Bigger with operator > should return false";
	//EXPECT_TRUE(value > xEqualYBigger) << "Comparing with X Equal and Y Bigger with operator > should return false";
	//EXPECT_TRUE(value > xEqualYLower) << "Comparing with X Equal and Y Bigger with operator > should return true";
	//EXPECT_TRUE(value > xBiggerYEqual) << "Comparing with X Equal and Y Bigger with operator > should return false";
	//EXPECT_TRUE(value > xLowerYEqual) << "Comparing with X Equal and Y Bigger with operator > should return true";

	//EXPECT_TRUE(value <= xEqualYEqual) << "Comparing with X Equal and Y Bigger with operator <= should return false";
	//EXPECT_TRUE(value <= xEqualYBigger) << "Comparing with X Equal and Y Bigger with operator <= should return false";
	//EXPECT_TRUE(value <= xEqualYLower) << "Comparing with X Equal and Y Bigger with operator <= should return true";
	//EXPECT_TRUE(value <= xBiggerYEqual) << "Comparing with X Equal and Y Bigger with operator <= should return false";
	//EXPECT_TRUE(value <= xLowerYEqual) << "Comparing with X Equal and Y Bigger with operator <= should return true";
	
	//EXPECT_TRUE(value >= xEqualYEqual) << "Comparing with X Equal and Y Bigger with operator >= should return false";
	//EXPECT_TRUE(value >= xEqualYBigger) << "Comparing with X Equal and Y Bigger with operator >= should return false";
	//EXPECT_TRUE(value >= xEqualYLower) << "Comparing with X Equal and Y Bigger with operator >= should return true";
	//EXPECT_TRUE(value >= xBiggerYEqual) << "Comparing with X Equal and Y Bigger with operator >= should return false";
	//EXPECT_TRUE(value >= xLowerYEqual) << "Comparing with X Equal and Y Bigger with operator >= should return true";
}

TEST_F(Vector2Test, ToStringMethod)
{
	EXPECT_EQ(spk::IVector2<int>(3, -4).to_string(), L"(3, -4)") << "to_string method should return correct string representation for int";
	EXPECT_EQ(spk::IVector2<float>(3.5f, 4.5f).to_string(), L"(3.5, 4.5)") << "to_string method should return correct string representation for float";
	EXPECT_EQ(spk::IVector2<double>(3.5, 4.5).to_string(), L"(3.5, 4.5)") << "to_string method should return correct string representation for double";
	EXPECT_EQ(spk::IVector2<size_t>(3, 4).to_string(), L"(3, 4)") << "to_string method should return correct string representation for size_t";
}