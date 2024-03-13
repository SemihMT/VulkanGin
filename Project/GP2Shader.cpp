#include "GP2Shader.h"

#include "vulkanbase/VulkanBase.h"
#include "vulkanbase/VulkanUtil.h"

GP2Shader::GP2Shader(const std::string& vertShaderPath, const std::string& fragShaderPath)
	: m_VertexShaderFile(vertShaderPath),
	m_FragmentShaderFile(fragShaderPath)
{

}

//General CreateShader function instead of different ones for the different shader stages
//VkPipelineShaderStageCreateInfo createShaderInfo(const std::string& shaderFile)
//{
//
//	std::vector<char> shaderCode = readFile(shaderFile);
//	VkShaderModule shaderModule = createShaderModule(shaderFile);
//
//	VkPipelineShaderStageCreateInfo shaderStageInfo{};
//	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//	shaderStageInfo.module = shaderModule;
//	shaderStageInfo.pName = "main";
//
//	return shaderStageInfo;
//}

void GP2Shader::initialize(const VkDevice& vkDevice)
{
	m_ShaderStages.emplace_back(createVertexShaderInfo(vkDevice));
	m_ShaderStages.emplace_back(createFragmentShaderInfo(vkDevice));
}

std::vector<VkPipelineShaderStageCreateInfo>& GP2Shader::getShaderStages()
{
	return m_ShaderStages;
}

void GP2Shader::destroyShaderModules(const VkDevice& vkDevice)
{
	for(auto& shaderStage : m_ShaderStages)
	{
		vkDestroyShaderModule(vkDevice, shaderStage.module, nullptr);
	}
	m_ShaderStages.clear();
}

VkPipelineShaderStageCreateInfo GP2Shader::createFragmentShaderInfo(const VkDevice& vkDevice) {
	std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = createShaderModule(vkDevice, fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo GP2Shader::createVertexShaderInfo(const VkDevice& vkDevice) {
	std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = createShaderModule(vkDevice, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo GP2Shader::createVertexInputStateInfo(const VkDevice& vkDevice)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	m_bindingDescription = Vertex::getBindingDescription();
	m_attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &m_bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo GP2Shader::createInputAssemblyStateInfo(const VkDevice& vkDevice)
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

//Helper to create a VKShaderModule
VkShaderModule GP2Shader::createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code) {

	//Object that stores a ptr to the buffer with the shader bytecode + its size
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();

	//code.Data() → char*
	//pCode → uint32_t
	//We have to reinterpret the ptr to satisfy the struct
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//Creating the actual shader module:
	//Shader modules are objects that contain some shader code and one or more entry points.
	//Shaders are selected from a shader module by specifying an entry point as part of pipeline creation
	//from:https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModule.html
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}