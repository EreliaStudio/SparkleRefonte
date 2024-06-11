#pragma once

#include <gtest/gtest.h>
#include "structure/math/spk_vector2.hpp"

template<typename TType>
class IVector2Test : public ::testing::Test
{
protected:
    IVector2<TType> vec1, vec2, vec3, vec4;

    void SetUp() override
    {
        vec1 = IVector2<TType>(1, 2);
        vec2 = IVector2<TType>(3, 4);
        vec3 = IVector2<TType>(5);
        vec4 = IVector2<TType>();
    }
};