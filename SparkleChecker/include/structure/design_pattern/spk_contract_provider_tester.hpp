#pragma once

#include "gtest/gtest.h"

#include "spk_define.hpp"
#include "structure/design_pattern/spk_contract_provider.hpp"

class ContractProviderTest : public ::testing::Test
{
protected:
    int executionCount;
    spk::ContractProvider::Job incrementCountJob;
    spk::ContractProvider provider;

    void SetUp() override
    {
        executionCount = 0;
        incrementCountJob = [this]() { executionCount++; };
    }
};