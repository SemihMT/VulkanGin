#pragma once
#include "GP2Shader.h"
#include "vulkan/vulkan_core.h"

template <typename VertexType>
class GP2GraphicsPipeline2D final
{
public:
	GP2GraphicsPipeline2D() = default;
	GP2GraphicsPipeline2D(VkDevice device, GP2Shader<VertexType>* shader, VkRenderPass renderPass, VkDescriptorSetLayout layout, VkPushConstantRange pushConstant)
	{
		Initialize(device, shader, renderPass, layout, pushConstant);
	}
	~GP2GraphicsPipeline2D() = default;


	void Initialize(VkDevice device, GP2Shader<VertexType>* shader, VkRenderPass renderPass, VkDescriptorSetLayout layout, VkPushConstantRange pushConstant)
	{
		m_device = device;
		m_shader = shader;
		m_renderPass = renderPass;
		m_descriptorSetLayout = layout;
		m_pushConstant = pushConstant;

		CreateGraphicsPipeline();
	}

	void CreateGraphicsPipeline()
	{
		createPipelineLayout();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_shader->getShaderStages().data();

		auto vertexInputState = m_shader->createVertexInputStateInfo(m_device);
		auto inputAssemblyState = m_shader->createInputAssemblyStateInfo(m_device);

		pipelineInfo.pVertexInputState = &vertexInputState;
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;

		VkPipelineViewportStateCreateInfo viewportState{};
		createViewportState(viewportState);

		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		createRasterizerState(rasterizerState);

		VkPipelineMultisampleStateCreateInfo multisampleState{};
		createMultisampleState(multisampleState);

		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		createColorBlendState(colorBlendState);

		VkPipelineDynamicStateCreateInfo dynamicState{};
		createDynamicState(dynamicState);

		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizerState;
		pipelineInfo.pMultisampleState = &multisampleState;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDynamicState = &dynamicState;

		
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		m_shader->destroyShaderModules(m_device);
	}

	void Destroy()
	{
		vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	}

	VkPipeline GetVkGraphicsPipeline() const
	{
		return m_graphicsPipeline;
	}

	VkPipelineLayout GetVkPipelineLayout() const
	{
		return m_pipelineLayout;
	}

private:

	void createViewportState(VkPipelineViewportStateCreateInfo& viewportState) {
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
	}

	void createRasterizerState(VkPipelineRasterizationStateCreateInfo& rasterizer) {
		
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
	}

	void createMultisampleState(VkPipelineMultisampleStateCreateInfo& multisampling) {
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	}

	void createColorBlendAttachment()
	{
		m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_colorBlendAttachment.blendEnable = VK_FALSE;
	}

	void createColorBlendState(VkPipelineColorBlendStateCreateInfo& colorBlending) {
		
		createColorBlendAttachment();
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &m_colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
	}

	void createDynamicState(VkPipelineDynamicStateCreateInfo& dynamicState) {
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
		dynamicState.pDynamicStates = m_dynamicStates.data();
	}

	void createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		//pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutInfo.pPushConstantRanges = &m_pushConstant;
		pipelineLayoutInfo.pushConstantRangeCount = 1;

		if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	//Things we need from outside the class
	GP2Shader<VertexType>* m_shader;
	VkDevice m_device;
	VkRenderPass m_renderPass;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPushConstantRange m_pushConstant;


	//Things that get created here
	VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
	VkPipeline m_graphicsPipeline{ VK_NULL_HANDLE };

	VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
	std::vector<VkDynamicState> m_dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
	};

};
