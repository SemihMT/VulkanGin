#pragma once

#include <array>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "vulkanbase/VulkanUtil.h"

template <typename VertexType>
class GP2Shader final
{
public:
	GP2Shader(const std::string& vertShaderPath, const std::string& fragShaderPath)
		: m_VertexShaderFile(vertShaderPath),
		m_FragmentShaderFile(fragShaderPath)
	{

	}

	~GP2Shader() = default;

	GP2Shader(const GP2Shader& other) = delete;
	GP2Shader(GP2Shader&& other) = delete;
	GP2Shader& operator=(const GP2Shader& other) = delete;
	GP2Shader& operator=(GP2Shader&& other) = delete;

	void initialize(const VkDevice& vkDevice)
	{
		m_ShaderStages.emplace_back(createVertexShaderInfo(vkDevice));
		m_ShaderStages.emplace_back(createFragmentShaderInfo(vkDevice));
	}

	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages()
	{
		return m_ShaderStages;
	}

	void destroyShaderModules(const VkDevice& vkDevice)
	{
		for (auto& shaderStage : m_ShaderStages)
		{
			vkDestroyShaderModule(vkDevice, shaderStage.module, nullptr);
		}
		m_ShaderStages.clear();
	}

	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(const VkDevice& vkDevice)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		m_bindingDescription = VertexType::getBindingDescription();
		m_attributeDescriptions = VertexType::getAttributeDescriptions();

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &m_bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();
		return vertexInputInfo;
	}

	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo(const VkDevice& vkDevice)
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		return inputAssembly;
	}

private:
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice)
	{
		std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
		VkShaderModule fragShaderModule = createShaderModule(vkDevice, fragShaderCode);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		return fragShaderStageInfo;
	}

	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice)
	{
		std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
		VkShaderModule vertShaderModule = createShaderModule(vkDevice, vertShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
		return vertShaderStageInfo;
	}

	VkShaderModule createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code)
	{
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

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;


	VkVertexInputBindingDescription m_bindingDescription{};
	std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions{};
};
