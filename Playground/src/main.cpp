#include "playground.hpp"

int main()
{
	//spk::cout.setPrefix(L"MainThread");
	
	spk::Thread* threadA = new spk::Thread(L"ThreadA", [&]() {
		for (size_t i = 0; i < 100000; i++)
		{
			spk::cout << "i : " << i << std::endl;
		}
		});
	spk::Thread* threadB = new spk::Thread(L"ThreadB", [&]() {
		for (size_t i = 0; i < 100000; i++)
		{
			spk::cout << "j : " << i << std::endl;
		}
		});

	threadA->start();
	DEBUG_LINE();
	threadB->start();

	threadA->join();
	threadB->join();

	return (0);


	/*spk::GraphicalApplication app;

	app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 400, 400));
	app.createWindow(L"SecondWindow", spk::Geometry2DInt(500, 0, 400, 400));

	return (app.run());*/
}