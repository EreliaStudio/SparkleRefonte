#include "playground.hpp"

int main()
{	
	spk::GraphicalApplication app(L"FirstWindow", spk::Geometry2DInt(0, 0, 800, 400));

	return (app.run());
}