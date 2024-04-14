#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanBase::drawScene2D() {
	//Replace with actual scene drawing code
	//foreach mesh in m_Scenes.current -> draw
	//vkCmdDrawIndexed(m_commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
	for (const auto& mesh : meshes2D)
	{
		mesh.Bind(m_commandBuffer);
		mesh.Draw(m_commandBuffer);
	}
}

void VulkanBase::drawScene3D() {
	//Replace with actual scene drawing code
	//foreach mesh in m_Scenes.current -> draw
	//vkCmdDrawIndexed(m_commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
	for (const auto& mesh : meshes3D)
	{
		mesh.Bind(m_commandBuffer);
		mesh.Draw(m_commandBuffer);
	}
}