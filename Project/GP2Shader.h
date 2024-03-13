#pragma once

#include <array>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vector>

class GP2Shader final
{
public:

	GP2Shader(const std::string& vertShaderPath, const std::string& fragShaderPath);
	~GP2Shader() = default;

	GP2Shader(const GP2Shader& other) = delete;
	GP2Shader(GP2Shader&& other) = delete;
	GP2Shader& operator=(const GP2Shader& other) = delete;
	GP2Shader& operator=(GP2Shader&& other) = delete;

	void initialize(const VkDevice& vkDevice);
	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages();
	void destroyShaderModules(const VkDevice& vkDevice);

	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(const VkDevice& vkDevice);
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo(const VkDevice& vkDevice);
private:
	
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice);
	VkShaderModule createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code);

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;


	VkVertexInputBindingDescription m_bindingDescription{};
	std::array<VkVertexInputAttributeDescription, 2> m_attributeDescriptions{};
};
