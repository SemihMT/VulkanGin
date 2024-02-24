#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

VkPipelineShaderStageCreateInfo VulkanBase::createFragmentShaderInfo() {
	std::vector<char> fragShaderCode = readFile("shaders/shader.frag.spv");
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo VulkanBase::createVertexShaderInfo() {
	std::vector<char> vertShaderCode = readFile("shaders/shader.vert.spv");
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo VulkanBase::createVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo VulkanBase::createInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

//Helper to create a VKShaderModule
VkShaderModule VulkanBase::createShaderModule(const std::vector<char>& code) {

	//Object that stores a ptr to the buffer with the shader bytecode + its size
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();

	//code.Data() → char*
	//pCode → uint32_t
	//We have to reinterpret the ptr to satisfy the struct
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//Creating the actual shader module:
	//Shader modules are objects that contain some shader code and on or more entry points.
	//Shaders are selected from a shader module by specifying an entry point as part of pipeline creation
	//from:https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModule.html
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void VulkanBase::drawScene() {
	vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}