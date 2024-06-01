#define STB_IMAGE_IMPLEMENTATION
//#include "vulkanbase/VulkanBase.h"

#include <iostream>

#include "vxl_app.h"

int main() {
	// DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
	//DISABLE_LAYER_NV_OPTIMUS_1 = 1
	//_putenv_s("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
	//_putenv_s("DISABLE_LAYER_NV_OPTIMUS_1", "1");
	//VulkanBase app;

	

	vxl::vxlApp vxlGame{};
	try {
		//app.Run();
		vxlGame.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}