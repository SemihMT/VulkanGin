#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <stdexcept>

#include "vxl_app.h"



namespace vxl
{
	void vxlApp::PrintControls()
	{
		std::cout << "\033[1;36m--- Controls ---\033[0m\n";
		std::cout << "\033[1;36mWASD\033[0m: Movement\n";
		std::cout << "\033[1;36m1-9\033[0m: Select blocks in order:\n";
		std::cout << "  \033[1;34m1\033[0m: Stone\n";
		std::cout << "  \033[1;34m2\033[0m: Cobblestone\n";
		std::cout << "  \033[1;34m3\033[0m: Bricks\n";
		std::cout << "  \033[1;34m4\033[0m: Dirt\n";
		std::cout << "  \033[1;34m5\033[0m: Oak Planks\n";
		std::cout << "  \033[1;34m6\033[0m: Oak Log\n";
		std::cout << "  \033[1;34m7\033[0m: Leaves\n";
		std::cout << "  \033[1;34m8\033[0m: Glass\n";
		std::cout << "  \033[1;34m9\033[0m: Sand\n";
		std::cout << "\033[1;36mScroll Wheel\033[0m: Increase/Decrease FOV\n";
		std::cout << "\033[1;36mSpacebar\033[0m: Move Up\n";
		std::cout << "\033[1;36mLeft Shift\033[0m: Move Down\n";
		std::cout << "\033[1;36mLeft Control\033[0m: Increase Speed\n";
	}

	vxlApp::vxlApp()
	{
		CreateUBO();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
		LoadModels();
		PrintControls();
	}

	vxlApp::~vxlApp()
	{
		vkDestroyPipelineLayout(m_vxlDevice.GetDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipelineLayout(m_vxlDevice.GetDevice(), m_pipelineLayout3D, nullptr);


	}

	void vxlApp::Run()
	{
		using namespace std::chrono;
		using clock = steady_clock;

		auto lastFrameTime = clock::now();
		while (!m_vxlWindow.ShouldClose())
		{
			auto currentFrameTime = clock::now();
			duration<float> deltaTime = duration_cast<duration<float>>(currentFrameTime - lastFrameTime);
			lastFrameTime = currentFrameTime;

			glfwPollEvents();
			m_vxlCamera.Update(deltaTime.count());
			m_vxlWorld->Update(deltaTime.count());
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


		glm::vec2 hotbarTextureSize{ 414,45 };
		glm::vec2 meshTopLeft{ (WIDTH - hotbarTextureSize.x) / 2,hotbarTextureSize.y };
		glm::vec2 meshBottomLeft{ (WIDTH - hotbarTextureSize.x) / 2,0 };
		glm::vec2 meshTopRight{ WIDTH - (WIDTH - hotbarTextureSize.x) / 2,hotbarTextureSize.y };
		glm::vec2 meshBottomRight{ WIDTH - (WIDTH - hotbarTextureSize.x) / 2,0 };
		std::vector<vxlModel::Vertex> vertices
		{
			{ScreenSpaceToNDC(meshTopLeft, WIDTH, HEIGHT), {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ScreenSpaceToNDC(meshBottomLeft, WIDTH, HEIGHT), {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },
			{ScreenSpaceToNDC(meshBottomRight, WIDTH, HEIGHT), {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
			{ScreenSpaceToNDC(meshTopRight, WIDTH, HEIGHT), {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} }
		};

		const std::vector<uint32_t> indices =
		{
			 0, 1, 2, 2, 3, 0
		};
		m_vxlHotbarModel = std::make_unique<vxlModel>(m_vxlDevice, vertices, indices);

		glm::vec2 crosshairSize{ 32 };
		glm::vec2 screenCenter{ WIDTH / 2, HEIGHT / 2 };

		meshTopLeft = { screenCenter.x - crosshairSize.x / 2, screenCenter.y + crosshairSize.y / 2 };
		meshBottomLeft = { screenCenter.x - crosshairSize.x / 2, screenCenter.y - crosshairSize.y / 2 };
		meshTopRight = { screenCenter.x + crosshairSize.x / 2, screenCenter.y + crosshairSize.y / 2 };
		meshBottomRight = { screenCenter.x + crosshairSize.x / 2, screenCenter.y - crosshairSize.y / 2 };
		std::vector<vxlModel::Vertex> crosshairVertices
		{
		{ScreenSpaceToNDC(meshTopLeft,WIDTH,HEIGHT), {1.0f, 0.0f, 0.0f},{0.0f,1.0f} },
		{ScreenSpaceToNDC(meshBottomLeft,WIDTH,HEIGHT), {0.0f, 1.0f, 0.0f},{0.0f,0.0f}},
		{ScreenSpaceToNDC(meshBottomRight,WIDTH,HEIGHT), {0.0f, 0.0f, 1.0f},{1.0f,0.0f}},
		{ScreenSpaceToNDC(meshTopRight,WIDTH,HEIGHT), {1.0f, 1.0f, 1.0f},{1.0f,1.0f}}
		};

		const std::vector<uint32_t> crosshairIndices =
		{
			0, 1, 2, 2, 3, 0
		};
		m_vxlCrosshairModel = std::make_unique<vxlModel>(m_vxlDevice, crosshairVertices, crosshairIndices);
		m_vxlWorld = std::make_unique<vxlWorld>(m_vxlDevice, &m_vxlCamera);

	}
	void vxlApp::CreateUBO()
	{
		m_vxlUBO = std::make_unique<vxlUBO>(m_vxlDevice);
	}

	void vxlApp::CreatePipelineLayout()
	{
		const auto descSetLayout = m_vxlUBO->GetDescriptorSetLayout();
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_vxlDevice.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}


		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo3D{};
		pipelineLayoutCreateInfo3D.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo3D.setLayoutCount = 1;

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
		m_vxlUBO->UpdateUBO(static_cast<uint32_t>(m_vxlSwapChain->GetCurrentFrame()), m_vxlSwapChain->GetSwapChainExtent(), m_vxlCamera);

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
		clearValues[0].color = { {0.390625f, 0.58203125f, 0.92578125f, 1.0f} };
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

		//m_vxlModel3D->Bind(m_commandBuffers[imageIdx]);
		m_vxlUBO->Bind(m_commandBuffers[imageIdx], m_pipelineLayout3D, static_cast<uint32_t>(m_vxlSwapChain->GetCurrentFrame()));
		m_vxlWorld->Draw(m_commandBuffers[imageIdx]);
		//m_vxlModel3D->Draw(m_commandBuffers[imageIdx]);

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


		m_vxlUBO->Bind(m_commandBuffers[imageIdx], m_pipelineLayout, static_cast<uint32_t>(m_vxlSwapChain->GetCurrentFrame()));
		m_vxlHotbarModel->Bind(m_commandBuffers[imageIdx]);
		m_vxlHotbarModel->Draw(m_commandBuffers[imageIdx]);

		m_vxlCrosshairModel->Bind(m_commandBuffers[imageIdx]);
		m_vxlCrosshairModel->Draw(m_commandBuffers[imageIdx]);

		vkCmdEndRenderPass(m_commandBuffers[imageIdx]);

		if (vkEndCommandBuffer(m_commandBuffers[imageIdx]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
	}
}
