#include "vxl_app.h"

#include <array>
#include <chrono>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
namespace vxl
{
	vxlApp::vxlApp()
	{
		LoadModels();
		CreateUBO();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	vxlApp::~vxlApp()
	{
		vkDestroyPipelineLayout(m_vxlDevice.GetDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipelineLayout(m_vxlDevice.GetDevice(), m_pipelineLayout3D,nullptr);
	}

	void vxlApp::Run()
	{
		while (!m_vxlWindow.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(m_vxlDevice.GetDevice());
	}

	void vxlApp::LoadModels()
	{
	/*	std::vector<vxlModel::Vertex> vertices
		{
		{{-0.5f, 0.8f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.8f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 1.f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 1.f}, {1.0f, 1.0f, 1.0f}}
		};*/

		std::vector<vxlModel::Vertex> vertices
		{
		{ScreenSpaceToNDC({200,100},WIDTH,HEIGHT), {1.0f, 0.0f, 0.0f} },
		{ScreenSpaceToNDC({200,0},WIDTH,HEIGHT), {0.0f, 1.0f, 0.0f}},
		{ScreenSpaceToNDC({600,0},WIDTH,HEIGHT), {0.0f, 0.0f, 1.0f}},
		{ScreenSpaceToNDC({600,100},WIDTH,HEIGHT), {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint32_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		m_vxlModel = std::make_unique<vxlModel>(m_vxlDevice, vertices,indices);


		std::vector<vxlModel3D::Vertex3D> vertices3D
		{
			// left face (white)
			 {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			 {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			 {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			 {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			 {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			 {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			 // right face (yellow)
			 {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			 {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			 {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			 {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			 {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			 {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			 // top face (orange, remember y axis points down)
			 {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			 {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			 {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			 {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			 {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			 {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			 // bottom face (red)
			 {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			 {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			 {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			 {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			 {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			 {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			 // nose face (blue)
			 {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			 {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			 {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			 {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			 {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			 {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			 // tail face (green)
			 {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			 {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			 {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			 {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			 {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			 {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		};

		const std::vector<unsigned int> indices3D = {
			// Left face (white)
			0, 1, 2,
			3, 4, 5,

			// Right face (yellow)
			6, 7, 8,
			9, 10, 11,

			// Top face (orange)
			12, 13, 14,
			15, 16, 17,

			// Bottom face (red)
			18, 19, 20,
			21, 22, 23,

			// Nose face (blue)
			24, 25, 26,
			27, 28, 29,

			// Tail face (green)
			30, 31, 32,
			33, 34, 35
		};

		m_vxlModel3D = std::make_unique<vxlModel3D>(m_vxlDevice, vertices3D, indices3D);
	}

	void vxlApp::CreateUBO()
	{
		m_vxlUBO = std::make_unique<vxlUBO>(m_vxlDevice);
	}

	void vxlApp::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_vxlDevice.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}


		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo3D{};
		pipelineLayoutCreateInfo3D.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo3D.setLayoutCount = 1;

		const auto descSetLayout = m_vxlUBO->GetDescriptorSetLayout();
		pipelineLayoutCreateInfo3D.pSetLayouts = &descSetLayout;
		pipelineLayoutCreateInfo3D.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo3D.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_vxlDevice.GetDevice(), &pipelineLayoutCreateInfo3D, nullptr, &m_pipelineLayout3D) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void vxlApp::CreatePipeline()
	{
		assert(m_vxlSwapChain != nullptr && "Can't create pipeline before swapchain!");
		assert(m_pipelineLayout != nullptr && "Can't create pipeline before pipeline layout!");

		PipelineConfiguration pipelineConfig{};
		vxlGraphicsPipeline::CreateDefaultPipelineConfiguration(pipelineConfig);

		pipelineConfig.renderPass = m_vxlSwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_vxlGraphicsPipeline = std::make_unique<vxlGraphicsPipeline>(
			m_vxlDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);


		PipelineConfiguration pipelineConfig3D{};
		vxlGraphicsPipeline3D::CreateDefaultPipelineConfiguration(pipelineConfig3D);

		pipelineConfig3D.renderPass = m_vxlSwapChain->GetRenderPass3D();
		pipelineConfig3D.descriptorSetLayout = m_vxlUBO->GetDescriptorSetLayout();
		pipelineConfig3D.pipelineLayout = m_pipelineLayout3D;
		m_vxlGraphicsPipeline3D = std::make_unique<vxlGraphicsPipeline3D>(
			m_vxlDevice,
			"shaders/simple_shader3D.vert.spv",
			"shaders/simple_shader3D.frag.spv",
			pipelineConfig3D);
	}

	void vxlApp::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_vxlSwapChain->ImageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_vxlDevice.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

		if (vkAllocateCommandBuffers(m_vxlDevice.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffer!");
		}

	}

	void vxlApp::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_vxlDevice.GetDevice(), m_vxlDevice.GetCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void vxlApp::DrawFrame()
	{
		uint32_t imageIdx;
		auto result = m_vxlSwapChain->AcquireNextImage(&imageIdx);
		m_vxlUBO->UpdateUBO(m_vxlSwapChain->GetCurrentFrame(), m_vxlSwapChain->GetSwapChainExtent());

		//VK_ERROR_OUT_OF_DATE_KHR: A surface has changed in such a way that it is no longer compatible with the current swapchain
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swapchain image");
		}

		RecordCommandBuffer(imageIdx);
		result = m_vxlSwapChain->SubmitCommandBuffers(&m_commandBuffers[imageIdx], &imageIdx);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_vxlWindow.WasWindowResized())
		{
			m_vxlWindow.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swapchain image");
		}
	}

	void vxlApp::RecreateSwapChain()
	{
		auto extent = m_vxlWindow.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			//pauses the window when it's not visible (e.g. when minimized)
			extent = m_vxlWindow.GetExtent();
			glfwWaitEvents();
		}

		//Wait until the current swapchain is not in use
		vkDeviceWaitIdle(m_vxlDevice.GetDevice());

		if (m_vxlSwapChain == nullptr)
		{
			m_vxlSwapChain = std::make_unique<vxlSwapChain>(m_vxlDevice, extent);
		}
		else
		{
			m_vxlSwapChain.reset(nullptr); //On some systems two swap chains can't coexist on the same window, so this ensures the old swap chain is destroyed first.
			m_vxlSwapChain = std::make_unique<vxlSwapChain>(m_vxlDevice, extent, std::move(m_vxlSwapChain));
			if (m_vxlSwapChain->ImageCount() != m_commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		//TODO: Check if previous and new pipeline's render pass is compatible
		//If yes, do not recreate the pipeline
		CreatePipeline();

	}

	void vxlApp::RecordCommandBuffer(int imageIdx)
	{
		vkResetCommandBuffer(m_commandBuffers[imageIdx], 0);
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers[imageIdx], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording to command buffer!");
		}
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.1f,0.1f,0.1f,1.0f} };
		clearValues[1].depthStencil = { 1.0f,0 };


		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_vxlSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_vxlSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ {0,0}, m_vxlSwapChain->GetSwapChainExtent() };

		// 3D render pass
		VkRenderPassBeginInfo renderPassBeginInfo3D{};
		renderPassBeginInfo3D.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo3D.renderPass = m_vxlSwapChain->GetRenderPass3D(); // Create a separate render pass for the second render pass
		renderPassBeginInfo3D.framebuffer = m_vxlSwapChain->GetFrameBuffer3D(imageIdx); // Create a separate framebuffer for the second render pass

		renderPassBeginInfo3D.renderArea.offset = { 0,0 };
		renderPassBeginInfo3D.renderArea.extent = m_vxlSwapChain->GetSwapChainExtent();
		renderPassBeginInfo3D.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo3D.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[imageIdx], &renderPassBeginInfo3D, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(m_commandBuffers[imageIdx], 0, 1, &viewport);
		vkCmdSetScissor(m_commandBuffers[imageIdx], 0, 1, &scissor);

		m_vxlGraphicsPipeline3D->Bind(m_commandBuffers[imageIdx]);

		m_vxlModel3D->Bind(m_commandBuffers[imageIdx]);
		m_vxlUBO->Bind(m_commandBuffers[imageIdx], m_pipelineLayout3D, m_vxlSwapChain->GetCurrentFrame());
		m_vxlModel3D->Draw(m_commandBuffers[imageIdx]);

		vkCmdEndRenderPass(m_commandBuffers[imageIdx]);

		// 2D render pass
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_vxlSwapChain->GetRenderPass();
		renderPassBeginInfo.framebuffer = m_vxlSwapChain->GetFrameBuffer(imageIdx);

		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = m_vxlSwapChain->GetSwapChainExtent();

		
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[imageIdx], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(m_commandBuffers[imageIdx], 0, 1, &viewport);
		vkCmdSetScissor(m_commandBuffers[imageIdx], 0, 1, &scissor);

		m_vxlGraphicsPipeline->Bind(m_commandBuffers[imageIdx]);

		m_vxlModel->Bind(m_commandBuffers[imageIdx]);
		m_vxlModel->Draw(m_commandBuffers[imageIdx]);

		vkCmdEndRenderPass(m_commandBuffers[imageIdx]);

		//// Transition resources for the second render pass
		//VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//VkDependencyFlags dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		//VkImageMemoryBarrier barrier{};
		//barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//barrier.image = m_vxlSwapChain->GetSwapChainImages()[imageIdx];
		//barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//barrier.subresourceRange.baseMipLevel = 0;
		//barrier.subresourceRange.levelCount = 1;
		//barrier.subresourceRange.baseArrayLayer = 0;
		//barrier.subresourceRange.layerCount = 1;
		//barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

		//vkCmdPipelineBarrier(
		//	m_commandBuffers[imageIdx],
		//	srcStage, dstStage,
		//	dependencyFlags,
		//	0, nullptr,
		//	0, nullptr,
		//	1, &barrier
		//);

		

		if (vkEndCommandBuffer(m_commandBuffers[imageIdx]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
	}
}
