#include "vxl_GraphicsPipeline3D.h"
#include "vxl_Model3D.h"
#include <fstream>
#include <cassert>

#include "vxl_Chunk.h"

namespace vxl
{

	vxlGraphicsPipeline3D::vxlGraphicsPipeline3D(vxlDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfiguration& config) :
		m_device{ device }
	{
		CreateGraphicsPipeline(vertFilePath, fragFilePath, config);
	}

	vxlGraphicsPipeline3D::~vxlGraphicsPipeline3D()
	{
		vkDestroyShaderModule(m_device.GetDevice(), m_vertShaderModule, nullptr);
		vkDestroyShaderModule(m_device.GetDevice(), m_fragShaderModule, nullptr);
		vkDestroyPipeline(m_device.GetDevice(), m_graphicsPipeline, nullptr);
	}

	void vxlGraphicsPipeline3D::Bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
	}

	void vxlGraphicsPipeline3D::CreateDefaultPipelineConfiguration(PipelineConfiguration& config)
	{

		// INPUT ASSEMBLY
		config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


		config.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config.viewportInfo.viewportCount = 1;
		config.viewportInfo.pViewports = nullptr;
		config.viewportInfo.scissorCount = 1;
		config.viewportInfo.pScissors = nullptr;


		// RASTERIZER
		config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config.rasterizationInfo.depthClampEnable = VK_FALSE;
		config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		config.rasterizationInfo.lineWidth = 1.0f;
		config.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		config.rasterizationInfo.depthBiasEnable = VK_FALSE;
		config.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		config.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		config.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		// MSAA
		config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config.multisampleInfo.sampleShadingEnable = VK_FALSE;
		config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config.multisampleInfo.minSampleShading = 1.0f;           // Optional
		config.multisampleInfo.pSampleMask = nullptr;             // Optional
		config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		config.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		// BLENDING
		config.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		config.colorBlendAttachment.blendEnable = VK_FALSE;
		config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config.colorBlendInfo.logicOpEnable = VK_FALSE;
		config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		config.colorBlendInfo.attachmentCount = 1;
		config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
		config.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		config.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		config.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		config.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		// DEPTH STENCIL
		config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config.depthStencilInfo.depthTestEnable = VK_TRUE;
		config.depthStencilInfo.depthWriteEnable = VK_TRUE;
		config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		config.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		config.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		config.depthStencilInfo.stencilTestEnable = VK_FALSE;
		config.depthStencilInfo.front = {};  // Optional
		config.depthStencilInfo.back = {};   // Optional

		config.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config.dynamicStateInfo.pDynamicStates = config.dynamicStateEnables.data();
		config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStateEnables.size());
		config.dynamicStateInfo.flags = 0;

	


	}

	std::vector<char> vxlGraphicsPipeline3D::ReadFile(const std::string& filePath)
	{
		// std::ios::ate -> seek to the end of file immediately
		// std::ios::binary -> read the file in as a binary file
		std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file: " + filePath);
		}

		//We're at the end of the file, tellg returns the position of the current character in the input stream.
		size_t fileSize = static_cast<size_t>(file.tellg());

		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void vxlGraphicsPipeline3D::CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfiguration& config)
	{
		//Read the shader code
		auto vertFileCode = ReadFile(vertFilePath);
		auto fragFileCode = ReadFile(fragFilePath);

		assert(config.pipelineLayout != VK_NULL_HANDLE && "Can not create graphics pipeline! No pipelineLayout provided in PipelineConfiguration!");
		assert(config.renderPass != VK_NULL_HANDLE && "Can not create graphics pipeline! No renderPass provided in PipelineConfiguration!");

		//Fill in the code related fields of the modules
		CreateShaderModule(vertFileCode, &m_vertShaderModule);
		CreateShaderModule(fragFileCode, &m_fragShaderModule);

		//Specify the modules further
		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = m_vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		//vertexInputInfo describes the format of the vertex data that will be passed to the vertex shader

		//Bindings: spacing between data and whether the data is per - vertex or per - instance(see instancing)
		//Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset

		const auto bindingDescriptions = vxlChunk::Vertex3D::GetBindingDescriptions();
		const auto attributeDescriptions = vxlChunk::Vertex3D::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
		pipelineInfo.pViewportState = &config.viewportInfo;
		pipelineInfo.pRasterizationState = &config.rasterizationInfo;
		pipelineInfo.pMultisampleState = &config.multisampleInfo;
		pipelineInfo.pColorBlendState = &config.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
		pipelineInfo.pDynamicState = &config.dynamicStateInfo;

		pipelineInfo.layout = config.pipelineLayout;
		pipelineInfo.renderPass = config.renderPass;
		pipelineInfo.subpass = config.subpass;

		//For when we want to create new pipelines inheriting from an existing one
		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		

		if (vkCreateGraphicsPipelines(m_device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}

	}

	void vxlGraphicsPipeline3D::CreateShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		if (vkCreateShaderModule(m_device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module");
		}
	}

}
