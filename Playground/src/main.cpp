#include "playground.hpp"

int main()
{	
	spk::GraphicalApplication app;

	spk::SafePointer<spk::Window> firstWindow = app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 800, 400));
	spk::SafePointer<spk::Window> secondWindow = app.createWindow(L"SecondWindow", spk::Geometry2DInt(0, 0, 400, 800));

	return (app.run());
}