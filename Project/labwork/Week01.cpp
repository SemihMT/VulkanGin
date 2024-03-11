#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanBase::drawScene() {
	//Replace with actual scene drawing code
	//foreach mesh in m_Scenes.current -> draw
	vkCmdDraw(m_commandBuffer.GetVkCommandBuffer(), 6, 1, 0, 0);
}