#include "application/spk_application_tester.hpp"

TEST_F(ApplicationTest, AddBehaviorToMainThread)
{
	auto contract = app.addBehavior(callback);

	ASSERT_TRUE(contract.isValid()) << "Contract should be valid after adding behavior to main thread.";
}

TEST_F(ApplicationTest, AddBehaviorToSpecificThread)
{
	std::wstring threadName = L"WorkerThread";

	auto contract = app.addBehavior(threadName, callback);

	ASSERT_TRUE(contract.isValid()) << "Contract should be valid after adding behavior to specific thread.";
}

TEST_F(ApplicationTest, RunApplication)
{
	auto contract = app.addBehavior(callback);

	int errorReturn = 0;
	std::thread runThread([this, &errorReturn]() { errorReturn = app.run(); });
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	int errorCode = 0;
	app.quit(errorCode);

	runThread.join();

	ASSERT_GT(counter.load(), 0) << "Callback should have been executed during run.";
	ASSERT_FALSE(app.isRunning()) << "Application should not be running after quit.";
	ASSERT_EQ(errorReturn, errorCode) << "Application should return the correct error code after quit.";
}

TEST_F(ApplicationTest, QuitApplicationWithError)
{
	auto contract = app.addBehavior(callback);

	int errorReturn = 0;
	std::thread runThread([this, &errorReturn]() { errorReturn = app.run(); });
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	int errorCode = 42;
	app.quit(errorCode);

	runThread.join();

	ASSERT_FALSE(app.isRunning()) << "Application should not be running after quit.";
	ASSERT_EQ(errorReturn, errorCode) << "Application should return the correct error code after quit.";
}