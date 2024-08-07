#include <iostream>
#include "vxl_app.h"

int main()
{
	//Initialize the Game application
	vxl::vxlApp vxlGame{};
	try
	{
		//Start the application (game loop)
		vxlGame.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}