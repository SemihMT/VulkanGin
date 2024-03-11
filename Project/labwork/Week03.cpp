#include "vulkanbase/VulkanBase.h"

void VulkanBase::createFrameBuffers() {
	//Resize the vector that holds the framebuffers
	//we need one for each image in our swapchain
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}



void VulkanBase::createRenderPass() {
	//Color attachment.This is the description of the image we will be writing into with rendering commands.
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat; //rendering the image should be compliant with the swapchain
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //no multisampling - so one bit
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear the buffer before rendering to it
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //Store the rendered image in memory
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // no stencil stuff so we don't care
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //ditto
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The initial layout is no known
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // after the renderpass ends, the layout will be ready to be displayed with the swapchain

	//Now that our main image target is defined, we need to add a subpass that will render into it.
	//This goes right after defining the attachment


	//Subpasses 'reference' one or more attachments that we described above
	//We'll be using the attachments as color buffers so define the layout as such
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	//The image life will go something like this:
	//UNDEFINED->RenderPass Begins->Subpass 0 begins(Transition to Attachment Optimal)
	//->Subpass 0 renders->Subpass 0 ends->Renderpass Ends(Transitions to Present Source)

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanBase::createGraphicsPipeline() {

	//Let the pipeline know how many viewports and scissors we'll use
	//This can be done without recreating the pipeline because we use dynamic state, see below
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	//The rasterizer, Creates the fragments from the geometry created by the vert shader
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	//Multisampling for anti-aliasing
	//Disabled for now
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//Setup color blending between the frambuffer color and the fragment colors returned by the rasterizer
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE; //Not blending any colors
	/*FOR ALPHA BLENDING:
	 *colorBlendAttachment.blendEnable = VK_TRUE;
	 *colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	 *colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	 *colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	 *colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	 *colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	 *colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	 */

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	//Creating Dynamic State:
	//The following specifies that we want to leave out
	//Size of the viewport, line width and blend constants
	//As these can be modified at runtime without recreating the pipeline
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//Pipeline layout - important for uniform values for shaders
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	/*VkPipelineShaderStageCreateInfo vertShaderStageInfo = createVertexShaderInfo();
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = createFragmentShaderInfo();

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo,
		fragShaderStageInfo
	};*/

	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = m_GradientShader.getShaderStages().data();

	//Describing Vertex Input info in the Shader class we made
	//Because we have no vertex buffer, there is no actual vertex data here
	auto is = m_GradientShader.createVertexInputStateInfo(device);
	//Describing how we assemble the vertices we get (Triangle List)
	auto ias = m_GradientShader.createInputAssemblyStateInfo(device);

	pipelineInfo.pVertexInputState = &is;
	pipelineInfo.pInputAssemblyState = &ias;

	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	m_GradientShader.destroyShaderModules(device);
}