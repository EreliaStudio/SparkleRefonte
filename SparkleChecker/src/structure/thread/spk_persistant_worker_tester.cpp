#include "structure/thread/spk_persistant_worker_tester.hpp"

TEST_F(PersistantWorkerTest, WorkerInitialization)
{
	spk::PersistantWorker worker(workerName, callback);
	ASSERT_FALSE(worker.isJoinable()) << "Worker should not be joinable upon initialization.";
}

TEST_F(PersistantWorkerTest, WorkerStart)
{
	spk::PersistantWorker worker(workerName, callback);
	worker.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	ASSERT_TRUE(worker.isJoinable()) << "Worker should be joinable after start.";
}

TEST_F(PersistantWorkerTest, WorkerStop)
{
	spk::PersistantWorker worker(workerName, callback);
	worker.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	worker.stop();
	ASSERT_FALSE(worker.isJoinable()) << "Worker should not be joinable after stop.";
	int finalCount = counter.load();
	ASSERT_GT(finalCount, 0) << "Callback should have been executed multiple times.";
}

TEST_F(PersistantWorkerTest, WorkerStopWithoutStart)
{
	spk::PersistantWorker worker(workerName, callback);
	worker.stop();
	ASSERT_FALSE(worker.isJoinable()) << "Worker should not be joinable if stop is called without start.";
	ASSERT_EQ(counter.load(), 0) << "Callback should not have been executed if stop is called without start.";
}

TEST_F(PersistantWorkerTest, WorkerStartStopStart)
{
	spk::PersistantWorker worker(workerName, callback);
	worker.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	worker.stop();
	ASSERT_FALSE(worker.isJoinable()) << "Worker should not be joinable after first stop.";
	int firstCount = counter.load();
	ASSERT_GT(firstCount, 0) << "Callback should have been executed multiple times before first stop.";

	worker.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	worker.stop();
	ASSERT_FALSE(worker.isJoinable()) << "Worker should not be joinable after second stop.";
	int secondCount = counter.load();
	ASSERT_GT(secondCount, firstCount) << "Callback should have been executed additional times after second start.";
}