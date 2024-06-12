#include "structure/math/spk_vector3_tester.hpp"

TEST_F(Vector3Test, DefaultConstructor)
{
	spk::IVector3<int> vecInt;
	EXPECT_EQ(vecInt.x, 0) << "Default constructor for int should initialize x to 0";
	EXPECT_EQ(vecInt.y, 0) << "Default constructor for int should initialize y to 0";
	EXPECT_EQ(vecInt.z, 0) << "Default constructor for int should initialize z to 0";

	spk::IVector3<float> vecFloat;
	EXPECT_EQ(vecFloat.x, 0.0f) << "Default constructor for float should initialize x to 0.0f";
	EXPECT_EQ(vecFloat.y, 0.0f) << "Default constructor for float should initialize y to 0.0f";
	EXPECT_EQ(vecFloat.z, 0.0f) << "Default constructor for float should initialize z to 0.0f";

	spk::IVector3<double> vecDouble;
	EXPECT_EQ(vecDouble.x, 0.0) << "Default constructor for double should initialize x to 0.0";
	EXPECT_EQ(vecDouble.y, 0.0) << "Default constructor for double should initialize y to 0.0";
	EXPECT_EQ(vecDouble.z, 0.0) << "Default constructor for double should initialize z to 0.0";

	spk::IVector3<size_t> vecSizeT;
	EXPECT_EQ(vecSizeT.x, 0) << "Default constructor for size_t should initialize x to 0";
	EXPECT_EQ(vecSizeT.y, 0) << "Default constructor for size_t should initialize y to 0";
	EXPECT_EQ(vecSizeT.z, 0) << "Default constructor for size_t should initialize z to 0";
}

TEST_F(Vector3Test, ScalarConstructor)
{
	spk::IVector3<int> vecInt(5);
	EXPECT_EQ(vecInt.x, 5) << "Scalar constructor for int should initialize x to 5";
	EXPECT_EQ(vecInt.y, 5) << "Scalar constructor for int should initialize y to 5";
	EXPECT_EQ(vecInt.z, 5) << "Scalar constructor for int should initialize z to 5";

	spk::IVector3<float> vecFloat(5.5f);
	EXPECT_FLOAT_EQ(vecFloat.x, 5.5f) << "Scalar constructor for float should initialize x to 5.5f";
	EXPECT_FLOAT_EQ(vecFloat.y, 5.5f) << "Scalar constructor for float should initialize y to 5.5f";
	EXPECT_FLOAT_EQ(vecFloat.z, 5.5f) << "Scalar constructor for float should initialize z to 5.5f";

	spk::IVector3<double> vecDouble(5.5);
	EXPECT_DOUBLE_EQ(vecDouble.x, 5.5) << "Scalar constructor for double should initialize x to 5.5";
	EXPECT_DOUBLE_EQ(vecDouble.y, 5.5) << "Scalar constructor for double should initialize y to 5.5";
	EXPECT_DOUBLE_EQ(vecDouble.z, 5.5) << "Scalar constructor for double should initialize z to 5.5";

	spk::IVector3<size_t> vecSizeT(5);
	EXPECT_EQ(vecSizeT.x, 5) << "Scalar constructor for size_t should initialize x to 5";
	EXPECT_EQ(vecSizeT.y, 5) << "Scalar constructor for size_t should initialize y to 5";
	EXPECT_EQ(vecSizeT.z, 5) << "Scalar constructor for size_t should initialize z to 5";
}

TEST_F(Vector3Test, ThreeValueConstructor)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	EXPECT_EQ(vecInt.x, 3) << "Three-value constructor for int should initialize x to 3";
	EXPECT_EQ(vecInt.y, 4) << "Three-value constructor for int should initialize y to 4";
	EXPECT_EQ(vecInt.z, 5) << "Three-value constructor for int should initialize z to 5";

	spk::IVector3<float> vecFloat(3.5f, 4.5f, 5.5f);
	EXPECT_FLOAT_EQ(vecFloat.x, 3.5f) << "Three-value constructor for float should initialize x to 3.5f";
	EXPECT_FLOAT_EQ(vecFloat.y, 4.5f) << "Three-value constructor for float should initialize y to 4.5f";
	EXPECT_FLOAT_EQ(vecFloat.z, 5.5f) << "Three-value constructor for float should initialize z to 5.5f";

	spk::IVector3<double> vecDouble(3.5, 4.5, 5.5);
	EXPECT_DOUBLE_EQ(vecDouble.x, 3.5) << "Three-value constructor for double should initialize x to 3.5";
	EXPECT_DOUBLE_EQ(vecDouble.y, 4.5) << "Three-value constructor for double should initialize y to 4.5";
	EXPECT_DOUBLE_EQ(vecDouble.z, 5.5) << "Three-value constructor for double should initialize z to 5.5";

	spk::IVector3<size_t> vecSizeT(3, 4, 5);
	EXPECT_EQ(vecSizeT.x, 3) << "Three-value constructor for size_t should initialize x to 3";
	EXPECT_EQ(vecSizeT.y, 4) << "Three-value constructor for size_t should initialize y to 4";
	EXPECT_EQ(vecSizeT.z, 5) << "Three-value constructor for size_t should initialize z to 5";
}

TEST_F(Vector3Test, CopyConstructor)
{
	spk::IVector3<int> vecInt1(3, 4, 5);
	spk::IVector3<int> vecInt2(vecInt1);
	EXPECT_EQ(vecInt2.x, 3) << "Copy constructor for int should copy x value";
	EXPECT_EQ(vecInt2.y, 4) << "Copy constructor for int should copy y value";
	EXPECT_EQ(vecInt2.z, 5) << "Copy constructor for int should copy z value";

	spk::IVector3<float> vecFloat1(3.5f, 4.5f, 5.5f);
	spk::IVector3<float> vecFloat2(vecFloat1);
	EXPECT_FLOAT_EQ(vecFloat2.x, 3.5f) << "Copy constructor for float should copy x value";
	EXPECT_FLOAT_EQ(vecFloat2.y, 4.5f) << "Copy constructor for float should copy y value";
	EXPECT_FLOAT_EQ(vecFloat2.z, 5.5f) << "Copy constructor for float should copy z value";

	spk::IVector3<double> vecDouble1(3.5, 4.5, 5.5);
	spk::IVector3<double> vecDouble2(vecDouble1);
	EXPECT_DOUBLE_EQ(vecDouble2.x, 3.5) << "Copy constructor for double should copy x value";
	EXPECT_DOUBLE_EQ(vecDouble2.y, 4.5) << "Copy constructor for double should copy y value";
	EXPECT_DOUBLE_EQ(vecDouble2.z, 5.5) << "Copy constructor for double should copy z value";

	spk::IVector3<size_t> vecSizeT1(3, 4, 5);
	spk::IVector3<size_t> vecSizeT2(vecSizeT1);
	EXPECT_EQ(vecSizeT2.x, 3) << "Copy constructor for size_t should copy x value";
	EXPECT_EQ(vecSizeT2.y, 4) << "Copy constructor for size_t should copy y value";
	EXPECT_EQ(vecSizeT2.z, 5) << "Copy constructor for size_t should copy z value";
}

TEST_F(Vector3Test, EqualityOperator)
{
	spk::IVector3<int> vecInt1(3, 4, 5);
	spk::IVector3<int> vecInt2(3, 4, 5);
	spk::IVector3<int> vecInt3(6, 7, 8);
	EXPECT_EQ(vecInt1, vecInt2) << "Equality operator should return true for equal int vectors";
	EXPECT_NE(vecInt1, vecInt3) << "Equality operator should return false for different int vectors";

	spk::IVector3<float> vecFloat1(3.0f, 4.0f, 5.0f);
	spk::IVector3<float> vecFloat2(3.0f, 4.0f, 5.0f);
	spk::IVector3<float> vecFloat3(6.0f, 7.0f, 8.0f);
	EXPECT_EQ(vecFloat1, vecFloat2) << "Equality operator should return true for equal float vectors";
	EXPECT_NE(vecFloat1, vecFloat3) << "Equality operator should return false for different float vectors";

	spk::IVector3<double> vecDouble1(3.0, 4.0, 5.0);
	spk::IVector3<double> vecDouble2(3.0, 4.0, 5.0);
	spk::IVector3<double> vecDouble3(6.0, 7.0, 8.0);
	EXPECT_EQ(vecDouble1, vecDouble2) << "Equality operator should return true for equal double vectors";
	EXPECT_NE(vecDouble1, vecDouble3) << "Equality operator should return false for different double vectors";

	spk::IVector3<size_t> vecSizeT1(3, 4, 5);
	spk::IVector3<size_t> vecSizeT2(3, 4, 5);
	spk::IVector3<size_t> vecSizeT3(6, 7, 8);
	EXPECT_EQ(vecSizeT1, vecSizeT2) << "Equality operator should return true for equal size_t vectors";
	EXPECT_NE(vecSizeT1, vecSizeT3) << "Equality operator should return false for different size_t vectors";

	spk::IVector3<int> vecInt4(3, 4, 5);
	spk::IVector3<float> vecFloat4(3.5f, 4.0f, 5.0f);

	EXPECT_TRUE(vecInt4 == vecFloat4) << "Upon casting float to int, vecFloat should be equal as 3.5 should be concidered as 3";
	EXPECT_FALSE(vecFloat4 == vecInt4) << "Upon casting int to float, vecInt should be equal as 3 should be concidered as 3.0 and therefor must be different";
}

TEST_F(Vector3Test, MixedTypeEqualityOperator)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	spk::IVector3<float> vecFloat(3.0f, 4.0f, 5.0f);
	spk::IVector3<double> vecDouble(3.0, 4.0, 5.0);
	spk::IVector3<size_t> vecSizeT(3, 4, 5);

	EXPECT_EQ(vecInt, vecFloat) << "Mixed-type equality operator should return true for equivalent int and float vectors";
	EXPECT_EQ(vecInt, vecDouble) << "Mixed-type equality operator should return true for equivalent int and double vectors";
	EXPECT_EQ(vecInt, vecSizeT) << "Mixed-type equality operator should return true for equivalent int and size_t vectors";
	EXPECT_EQ(vecFloat, vecDouble) << "Mixed-type equality operator should return true for equivalent float and double vectors";
	EXPECT_EQ(vecFloat, vecSizeT) << "Mixed-type equality operator should return true for equivalent float and size_t vectors";
	EXPECT_EQ(vecDouble, vecSizeT) << "Mixed-type equality operator should return true for equivalent double and size_t vectors";

	spk::IVector3<int> vecIntDifferent(6, 7, 8);
	spk::IVector3<float> vecFloatDifferent(6.0f, 7.0f, 8.0f);
	spk::IVector3<double> vecDoubleDifferent(6.0, 7.0, 8.0);
	spk::IVector3<size_t> vecSizeTDifferent(6, 7, 8);

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

TEST_F(Vector3Test, ComparatorOperators)
{
	spk::IVector3<int> value(5, 6, 7);
	spk::IVector3<int> xEqualYEqualZEqual(5, 6, 7);
	spk::IVector3<int> xEqualYEqualZBigger(5, 6, 8);
	spk::IVector3<int> xEqualYEqualZLower(5, 6, 6);
	spk::IVector3<int> xEqualYBiggerZEqual(5, 7, 7);
	spk::IVector3<int> xEqualYLowerZEqual(5, 5, 7);
	spk::IVector3<int> xBiggerYEqualZEqual(6, 6, 7);
	spk::IVector3<int> xLowerYEqualZEqual(4, 6, 7);

	EXPECT_FALSE(value < xEqualYEqualZEqual) << "Comparing with X Equal, Y Equal, and Z Equal with operator < should return false";
	EXPECT_TRUE(value < xEqualYEqualZBigger) << "Comparing with X Equal, Y Equal, and Z Bigger with operator < should return true";
	EXPECT_FALSE(value < xEqualYEqualZLower) << "Comparing with X Equal, Y Equal, and Z Lower with operator < should return false";
	EXPECT_TRUE(value < xEqualYBiggerZEqual) << "Comparing with X Equal, Y Bigger, and Z Equal with operator < should return true";
	EXPECT_FALSE(value < xEqualYLowerZEqual) << "Comparing with X Equal, Y Lower, and Z Equal with operator < should return false";
	EXPECT_TRUE(value < xBiggerYEqualZEqual) << "Comparing with X Bigger, Y Equal, and Z Equal with operator < should return true";
	EXPECT_FALSE(value < xLowerYEqualZEqual) << "Comparing with X Lower, Y Equal, and Z Equal with operator < should return false";

	EXPECT_FALSE(value > xEqualYEqualZEqual) << "Comparing with X Equal, Y Equal, and Z Equal with operator > should return false";
	EXPECT_FALSE(value > xEqualYEqualZBigger) << "Comparing with X Equal, Y Equal, and Z Bigger with operator > should return false";
	EXPECT_TRUE(value > xEqualYEqualZLower) << "Comparing with X Equal, Y Equal, and Z Lower with operator > should return true";
	EXPECT_FALSE(value > xEqualYBiggerZEqual) << "Comparing with X Equal, Y Bigger, and Z Equal with operator > should return false";
	EXPECT_TRUE(value > xEqualYLowerZEqual) << "Comparing with X Equal, Y Lower, and Z Equal with operator > should return true";
	EXPECT_FALSE(value > xBiggerYEqualZEqual) << "Comparing with X Bigger, Y Equal, and Z Equal with operator > should return false";
	EXPECT_TRUE(value > xLowerYEqualZEqual) << "Comparing with X Lower, Y Equal, and Z Equal with operator > should return true";

	EXPECT_TRUE(value <= xEqualYEqualZEqual) << "Comparing with X Equal, Y Equal, and Z Equal with operator <= should return true";
	EXPECT_TRUE(value <= xEqualYEqualZBigger) << "Comparing with X Equal, Y Equal, and Z Bigger with operator <= should return true";
	EXPECT_FALSE(value <= xEqualYEqualZLower) << "Comparing with X Equal, Y Equal, and Z Lower with operator <= should return false";
	EXPECT_TRUE(value <= xEqualYBiggerZEqual) << "Comparing with X Equal, Y Bigger, and Z Equal with operator <= should return true";
	EXPECT_FALSE(value <= xEqualYLowerZEqual) << "Comparing with X Equal, Y Lower, and Z Equal with operator <= should return false";
	EXPECT_TRUE(value <= xBiggerYEqualZEqual) << "Comparing with X Bigger, Y Equal, and Z Equal with operator <= should return true";
	EXPECT_FALSE(value <= xLowerYEqualZEqual) << "Comparing with X Lower, Y Equal, and Z Equal with operator <= should return false";

	EXPECT_TRUE(value >= xEqualYEqualZEqual) << "Comparing with X Equal, Y Equal, and Z Equal with operator >= should return true";
	EXPECT_FALSE(value >= xEqualYEqualZBigger) << "Comparing with X Equal, Y Equal, and Z Bigger with operator >= should return false";
	EXPECT_TRUE(value >= xEqualYEqualZLower) << "Comparing with X Equal, Y Equal, and Z Lower with operator >= should return true";
	EXPECT_FALSE(value >= xEqualYBiggerZEqual) << "Comparing with X Equal, Y Bigger, and Z Equal with operator >= should return false";
	EXPECT_TRUE(value >= xEqualYLowerZEqual) << "Comparing with X Equal, Y Lower, and Z Equal with operator >= should return true";
	EXPECT_FALSE(value >= xBiggerYEqualZEqual) << "Comparing with X Bigger, Y Equal, and Z Equal with operator >= should return false";
	EXPECT_TRUE(value >= xLowerYEqualZEqual) << "Comparing with X Lower, Y Equal, and Z Equal with operator >= should return true";
}

TEST_F(Vector3Test, ToStringMethod)
{
	EXPECT_EQ(spk::IVector3<int>(3, -4, 5).to_string(), L"(3, -4, 5)") << "to_string method should return correct string representation for int";
	EXPECT_EQ(spk::IVector3<float>(3.5f, 4.5f, 5.5f).to_string(), L"(3.5, 4.5, 5.5)") << "to_string method should return correct string representation for float";
	EXPECT_EQ(spk::IVector3<double>(3.5, 4.5, 5.5).to_string(), L"(3.5, 4.5, 5.5)") << "to_string method should return correct string representation for double";
	EXPECT_EQ(spk::IVector3<size_t>(3, 4, 5).to_string(), L"(3, 4, 5)") << "to_string method should return correct string representation for size_t";
}

TEST_F(Vector3Test, ArithmeticOperators)
{
	spk::IVector3<int> vecInt1(3, 4, 5);
	spk::IVector3<int> vecInt2(1, 2, 3);

	auto vecIntResult = vecInt1 + vecInt2;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(4, 6, 8)) << "Addition operator should add correctly for int";

	vecIntResult = vecInt1 - vecInt2;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(2, 2, 2)) << "Subtraction operator should subtract correctly for int";

	vecIntResult = vecInt1 * vecInt2;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(3, 8, 15)) << "Multiplication operator should multiply correctly for int";

	vecIntResult = vecInt1 / vecInt2;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(3, 2, 1)) << "Division operator should divide correctly for int";

	spk::IVector3<float> vecFloat1(3.0f, 4.0f, 5.0f);
	spk::IVector3<float> vecFloat2(1.0f, 2.0f, 3.0f);

	auto vecFloatResult = vecFloat1 + vecFloat2;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(4.0f, 6.0f, 8.0f)) << "Addition operator should add correctly for float";

	vecFloatResult = vecFloat1 - vecFloat2;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(2.0f, 2.0f, 2.0f)) << "Subtraction operator should subtract correctly for float";

	vecFloatResult = vecFloat1 * vecFloat2;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(3.0f, 8.0f, 15.0f)) << "Multiplication operator should multiply correctly for float";

	vecFloatResult = vecFloat1 / vecFloat2;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(3.0f, 2.0f, 5.0f / 3.0f)) << "Division operator should divide correctly for float";
}

TEST_F(Vector3Test, CompoundAssignmentOperators)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	spk::IVector3<int> vecIntOther(1, 2, 3);

	vecInt += vecIntOther;
	EXPECT_EQ(vecInt, spk::IVector3<int>(4, 6, 8)) << "Compound addition operator should add correctly for int";

	vecInt -= vecIntOther;
	EXPECT_EQ(vecInt, spk::IVector3<int>(3, 4, 5)) << "Compound subtraction operator should subtract correctly for int";

	vecInt *= vecIntOther;
	EXPECT_EQ(vecInt, spk::IVector3<int>(3, 8, 15)) << "Compound multiplication operator should multiply correctly for int";

	vecInt /= vecIntOther;
	EXPECT_EQ(vecInt, spk::IVector3<int>(3, 4, 5)) << "Compound division operator should divide correctly for int";

	spk::IVector3<float> vecFloat(3.0f, 4.0f, 5.0f);
	spk::IVector3<float> vecFloatOther(1.0f, 2.0f, 3.0f);

	vecFloat += vecFloatOther;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(4.0f, 6.0f, 8.0f)) << "Compound addition operator should add correctly for float";

	vecFloat -= vecFloatOther;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(3.0f, 4.0f, 5.0f)) << "Compound subtraction operator should subtract correctly for float";

	vecFloat *= vecFloatOther;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(3.0f, 8.0f, 15.0f)) << "Compound multiplication operator should multiply correctly for float";

	vecFloat /= vecFloatOther;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(3.0f, 4.0f, 5.0f)) << "Compound division operator should divide correctly for float";
}

TEST_F(Vector3Test, ArithmeticOperatorsWithScalar)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	int scalar = 2;

	auto vecIntResult = vecInt + scalar;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(5, 6, 7)) << "Addition operator with scalar should add correctly for int";

	vecIntResult = vecInt - scalar;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(1, 2, 3)) << "Subtraction operator with scalar should subtract correctly for int";

	vecIntResult = vecInt * scalar;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(6, 8, 10)) << "Multiplication operator with scalar should multiply correctly for int";

	vecIntResult = vecInt / scalar;
	EXPECT_EQ(vecIntResult, spk::IVector3<int>(1, 2, 2)) << "Division operator with scalar should divide correctly for int";

	spk::IVector3<float> vecFloat(3.0f, 4.0f, 5.0f);
	float scalarFloat = 2.0f;

	auto vecFloatResult = vecFloat + scalarFloat;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(5.0f, 6.0f, 7.0f)) << "Addition operator with scalar should add correctly for float";

	vecFloatResult = vecFloat - scalarFloat;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(1.0f, 2.0f, 3.0f)) << "Subtraction operator with scalar should subtract correctly for float";

	vecFloatResult = vecFloat * scalarFloat;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(6.0f, 8.0f, 10.0f)) << "Multiplication operator with scalar should multiply correctly for float";

	vecFloatResult = vecFloat / scalarFloat;
	EXPECT_EQ(vecFloatResult, spk::IVector3<float>(1.5f, 2.0f, 2.5f)) << "Division operator with scalar should divide correctly for float";
}

TEST_F(Vector3Test, CompoundAssignmentOperatorsWithScalar)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	int scalar = 2;

	vecInt += scalar;
	EXPECT_EQ(vecInt, spk::IVector3<int>(5, 6, 7)) << "Compound addition operator with scalar should add correctly for int";

	vecInt -= scalar;
	EXPECT_EQ(vecInt, spk::IVector3<int>(3, 4, 5)) << "Compound subtraction operator with scalar should subtract correctly for int";

	vecInt *= scalar;
	EXPECT_EQ(vecInt, spk::IVector3<int>(6, 8, 10)) << "Compound multiplication operator with scalar should multiply correctly for int";

	vecInt /= scalar;
	EXPECT_EQ(vecInt, spk::IVector3<int>(3, 4, 5)) << "Compound division operator with scalar should divide correctly for int";

	spk::IVector3<float> vecFloat(3.0f, 4.0f, 5.0f);
	float scalarFloat = 2.0f;

	vecFloat += scalarFloat;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(5.0f, 6.0f, 7.0f)) << "Compound addition operator with scalar should add correctly for float";

	vecFloat -= scalarFloat;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(3.0f, 4.0f, 5.0f)) << "Compound subtraction operator with scalar should subtract correctly for float";

	vecFloat *= scalarFloat;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(6.0f, 8.0f, 10.0f)) << "Compound multiplication operator with scalar should multiply correctly for float";

	vecFloat /= scalarFloat;
	EXPECT_EQ(vecFloat, spk::IVector3<float>(3.0f, 4.0f, 5.0f)) << "Compound division operator with scalar should divide correctly for float";
}

TEST_F(Vector3Test, DivisionByZero)
{
	spk::IVector3<int> vecInt(3, 4, 5);
	int zeroInt = 0;

	EXPECT_THROW(
		{
			spk::IVector3<int> result = vecInt / zeroInt;
		}, std::runtime_error) << "Division by zero for int should throw runtime_error";

	EXPECT_THROW(
		{
			vecInt /= zeroInt;
		}, std::runtime_error) << "Division assignment by zero for int should throw runtime_error";

	spk::IVector3<float> vecFloat(3.0f, 4.0f, 5.0f);
	float zeroFloat = 0.0f;

	EXPECT_THROW(
		{
			spk::IVector3<float> result = vecFloat / zeroFloat;
		}, std::runtime_error) << "Division by zero for float should throw runtime_error";

	EXPECT_THROW(
		{
			vecFloat /= zeroFloat;
		}, std::runtime_error) << "Division assignment by zero for float should throw runtime_error";

	spk::IVector3<double> vecDouble(3.0, 4.0, 5.0);
	double zeroDouble = 0.0;

	EXPECT_THROW(
		{
			spk::IVector3<double> result = vecDouble / zeroDouble;
		}, std::runtime_error) << "Division by zero for double should throw runtime_error";

	EXPECT_THROW(
		{
			vecDouble /= zeroDouble;
		}, std::runtime_error) << "Division assignment by zero for double should throw runtime_error";
}
