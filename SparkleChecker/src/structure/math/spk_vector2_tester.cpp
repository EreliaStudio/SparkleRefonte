#include "structure/math/spk_vector2_tester.hpp"

using TestTypes = ::testing::Types<int, float, double>;
TYPED_TEST_SUITE(IVector2Test, TestTypes);

TYPED_TEST(IVector2Test, DefaultConstructor)
{
    IVector2<TypeParam> defaultVec;
    EXPECT_EQ(defaultVec, IVector2<TypeParam>(0, 0));
}

TYPED_TEST(IVector2Test, ScalarConstructor)
{
    EXPECT_EQ(this->vec3, IVector2<TypeParam>(5, 5));
}

TYPED_TEST(IVector2Test, TwoValueConstructor)
{
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(1, 2));
}

TYPED_TEST(IVector2Test, CopyConstructor)
{
    IVector2<TypeParam> vecCopy(this->vec1);
    EXPECT_EQ(vecCopy, this->vec1);
}

TYPED_TEST(IVector2Test, CopyAssignment)
{
    IVector2<TypeParam> vecAssign;
    vecAssign = this->vec1;
    EXPECT_EQ(vecAssign, this->vec1);
}

TYPED_TEST(IVector2Test, ToString)
{
    EXPECT_EQ(this->vec1.to_string(), L"(1, 2)");
}

TYPED_TEST(IVector2Test, EqualityOperator)
{
    EXPECT_TRUE(this->vec1 == IVector2<TypeParam>(1, 2));
    EXPECT_FALSE(this->vec1 == this->vec2);
}

TYPED_TEST(IVector2Test, InequalityOperator)
{
    EXPECT_TRUE(this->vec1 != this->vec2);
    EXPECT_FALSE(this->vec1 != IVector2<TypeParam>(1, 2));
}

TYPED_TEST(IVector2Test, LessThanOperator)
{
    EXPECT_TRUE(this->vec1 < this->vec2);
    EXPECT_FALSE(this->vec2 < this->vec1);
}

TYPED_TEST(IVector2Test, GreaterThanOperator)
{
    EXPECT_TRUE(this->vec2 > this->vec1);
    EXPECT_FALSE(this->vec1 > this->vec2);
}

TYPED_TEST(IVector2Test, LessThanOrEqualOperator)
{
    EXPECT_TRUE(this->vec1 <= this->vec2);
    EXPECT_TRUE(this->vec1 <= IVector2<TypeParam>(1, 2));
    EXPECT_FALSE(this->vec2 <= this->vec1);
}

TYPED_TEST(IVector2Test, GreaterThanOrEqualOperator)
{
    EXPECT_TRUE(this->vec2 >= this->vec1);
    EXPECT_TRUE(this->vec1 >= IVector2<TypeParam>(1, 2));
    EXPECT_FALSE(this->vec1 >= this->vec2);
}

TYPED_TEST(IVector2Test, ComparisonWithScalar)
{
    IVector2<TypeParam> vec(2, 2);
    TypeParam scalar = 2;
    EXPECT_TRUE(vec == scalar);
    EXPECT_FALSE(vec != scalar);
    EXPECT_TRUE(vec <= scalar);
    EXPECT_TRUE(vec >= scalar);
    EXPECT_FALSE(vec < scalar);
    EXPECT_FALSE(vec > scalar);
}

TYPED_TEST(IVector2Test, Addition)
{
    IVector2<TypeParam> result = this->vec1 + this->vec2;
    EXPECT_EQ(result, IVector2<TypeParam>(4, 6));
}

TYPED_TEST(IVector2Test, Subtraction)
{
    IVector2<TypeParam> result = this->vec2 - this->vec1;
    EXPECT_EQ(result, IVector2<TypeParam>(2, 2));
}

TYPED_TEST(IVector2Test, Multiplication)
{
    IVector2<TypeParam> result = this->vec1 * this->vec2;
    EXPECT_EQ(result, IVector2<TypeParam>(3, 8));
}

TYPED_TEST(IVector2Test, Division)
{
    IVector2<TypeParam> result = this->vec2 / this->vec1;
    EXPECT_EQ(result, IVector2<TypeParam>(3, 2));
}

TYPED_TEST(IVector2Test, DivisionByZero)
{
    IVector2<TypeParam> zeroVector(0, 0);
    EXPECT_THROW(this->vec1 / zeroVector, std::domain_error);
}

TYPED_TEST(IVector2Test, AdditionWithScalar)
{
    IVector2<TypeParam> result = this->vec1 + 3;
    EXPECT_EQ(result, IVector2<TypeParam>(4, 5));
}

TYPED_TEST(IVector2Test, SubtractionWithScalar)
{
    IVector2<TypeParam> result = this->vec1 - 1;
    EXPECT_EQ(result, IVector2<TypeParam>(0, 1));
}

TYPED_TEST(IVector2Test, MultiplicationWithScalar)
{
    IVector2<TypeParam> result = this->vec1 * 2;
    EXPECT_EQ(result, IVector2<TypeParam>(2, 4));
}

TYPED_TEST(IVector2Test, DivisionWithScalar)
{
    IVector2<TypeParam> result = this->vec1 / 2;
    EXPECT_EQ(result, IVector2<TypeParam>(0.5, 1));
}

TYPED_TEST(IVector2Test, AdditionAssignmentWithVector)
{
    this->vec1 += this->vec2;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(4, 6));
}

TYPED_TEST(IVector2Test, SubtractionAssignmentWithVector)
{
    this->vec2 -= this->vec1;
    EXPECT_EQ(this->vec2, IVector2<TypeParam>(2, 2));
}

TYPED_TEST(IVector2Test, MultiplicationAssignmentWithVector)
{
    this->vec1 *= this->vec2;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(3, 8));
}

TYPED_TEST(IVector2Test, DivisionAssignmentWithVector)
{
    this->vec2 /= this->vec1;
    EXPECT_EQ(this->vec2, IVector2<TypeParam>(3, 2));
}

TYPED_TEST(IVector2Test, AdditionAssignmentWithScalar)
{
    this->vec1 += 3;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(4, 5));
}

TYPED_TEST(IVector2Test, SubtractionAssignmentWithScalar)
{
    this->vec1 -= 1;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(0, 1));
}

TYPED_TEST(IVector2Test, MultiplicationAssignmentWithScalar)
{
    this->vec1 *= 2;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(2, 4));
}

TYPED_TEST(IVector2Test, DivisionAssignmentWithScalar)
{
    this->vec1 /= 2;
    EXPECT_EQ(this->vec1, IVector2<TypeParam>(0.5, 1));
}