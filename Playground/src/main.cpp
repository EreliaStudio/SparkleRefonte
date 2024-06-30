#include "playground.hpp"

int main()
{
	DEBUG_LINE();
	spk::GraphicalApplication app;

	DEBUG_LINE();
	spk::SafePointer<spk::Window> firstWindow = app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 800, 400));
	DEBUG_LINE();
	spk::SafePointer<spk::Window> secondWindow = app.createWindow(L"SecondWindow", spk::Geometry2DInt(0, 200, 400, 800));

	DEBUG_LINE();

	int result = app.run();

	DEBUG_LINE();
	return (result);
}