#include "playground.hpp"

int main()
{	
	spk::GraphicalApplication app;

	app.createWindow(L"FirstWindow", spk::Geometry2DInt(0, 0, 400, 400));
	app.createWindow(L"SecondWindow", spk::Geometry2DInt(500, 0, 400, 400));

	return (app.run());
}