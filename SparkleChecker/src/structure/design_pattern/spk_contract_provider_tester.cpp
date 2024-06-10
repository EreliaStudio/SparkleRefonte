#include "structure/design_pattern/spk_contract_provider_tester.hpp"

TEST_F(ContractProviderTest, SubscribeJob)
{
    auto contract = provider.subscribe(incrementCountJob);

    int expectedExecutionCount = 0;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    contract.execute();

    expectedExecutionCount = 1;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 1 after calling execute() on the contract";
}

TEST_F(ContractProviderTest, ResignJob)
{
    auto contract = provider.subscribe(incrementCountJob);

    ASSERT_EQ(contract.isValid(), true) << "A contract should be valid when created";

    contract.resign();

    int expectedExecutionCount = 0;

    ASSERT_EQ(contract.isValid(), false) << "A contract should be invalid when resigned";
    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    ASSERT_THROW(contract.execute(), std::runtime_error) << "Executing a resigned contract should throw a runtime error";
}

TEST_F(ContractProviderTest, MultipleJobsExecution)
{
    auto contract1 = provider.subscribe(incrementCountJob);
    auto contract2 = provider.subscribe(incrementCountJob);

    int expectedExecutionCount = 0;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    contract1.execute();
    expectedExecutionCount = 1;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 1 after calling execute() on contract 1";

    contract2.execute();
    expectedExecutionCount = 2;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 2 after calling execute() on contract 2";
}

TEST_F(ContractProviderTest, UnsubscribeJob)
{
    auto contract = provider.subscribe(incrementCountJob);

    provider.unsubscribe(contract);

    int expectedExecutionCount = 0;

    ASSERT_EQ(contract.isValid(), false) << "A contract should be invalid when the provider unsubscribe it";
    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    ASSERT_THROW(contract.execute(), std::runtime_error) << "Executing an unsubscribed contract should throw a runtime error";
}

TEST_F(ContractProviderTest, ResignRemovesFromProvider)
{
    auto contract = provider.subscribe(incrementCountJob);

    contract.resign();

    int expectedExecutionCount = 0;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    ASSERT_THROW(contract.execute(), std::runtime_error) << "Executing a resigned contract should throw a runtime error";
    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should still be 0 after trying to execute a resigned contract";

    provider.unsubscribe(contract);

    ASSERT_THROW(contract.execute(), std::runtime_error) << "Executing an unsubscribed contract should throw a runtime error";
    ASSERT_NO_THROW(provider.trigger()) << "Triggering contract provider should not throw a runtime error";
}

TEST_F(ContractProviderTest, TriggerExecutesAllJobs)
{
    auto contract = provider.subscribe(incrementCountJob);
    auto contract2 = provider.subscribe(incrementCountJob);

    int expectedExecutionCount = 0;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    provider.trigger();

    expectedExecutionCount = 2;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 2 after calling trigger";
}

TEST_F(ContractProviderTest, DestructorResignsContract)
{
    int expectedExecutionCount = 0;
    {
        auto contract = provider.subscribe(incrementCountJob);

        ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

        contract.execute();
        expectedExecutionCount = 1;

        ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 1 after calling execute() on the contract";
    }

    expectedExecutionCount = 1;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 1 after contract goes out of scope and resigns";

    provider.trigger();

    expectedExecutionCount = 1;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should remain 1 after calling trigger() because the contract was resigned upon destruction";
}

TEST_F(ContractProviderTest, RelinquishJob)
{
    auto contract = provider.subscribe(incrementCountJob);

    int expectedExecutionCount = 0;

    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 at the start of the test";

    contract.relinquish();

    ASSERT_THROW(contract.execute(), std::runtime_error) << "Executing a relinquished contract should not throw a runtime error";

    expectedExecutionCount = 0;
    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 0 after calling execute() on the relinquished contract";

    provider.trigger();

    expectedExecutionCount = 1;
    ASSERT_EQ(executionCount, expectedExecutionCount) << "Execution count should be 1 after calling trigger() on the provider";
}