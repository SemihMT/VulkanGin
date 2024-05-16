#pragma once
#include "vxl_Device.h"
#include "vulkan/vulkan.h"
#include "vxl_Utils.h"

#include <string>
#include <vector>
#include <memory>

namespace vxl
{
	class vxlSwapChain
	{
	public:
		vxlSwapChain(vxlDevice& device, VkExtent2D windowExtent);
		vxlSwapChain(vxlDevice& device, VkExtent2D windowExtent, std::shared_ptr<vxlSwapChain> previous);
		~vxlSwapChain();

		vxlSwapChain(const vxlSwapChain& other) = delete;
		vxlSwapChain(vxlSwapChain&& other) = delete;
		vxlSwapChain& operator=(const vxlSwapChain& other) = delete;
		vxlSwapChain& operator=(vxlSwapChain&& other) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return m_swapChainFrameBuffers[index]; }
		VkFramebuffer GetFrameBuffer3D(int index) { return m_swapChainFrameBuffers3D[index]; }
		VkImageView GetImageView(int index) { return m_swapChainImageViews[index]; }
		VkRenderPass GetRenderPass() { return m_renderPass; }
		VkRenderPass GetRenderPass3D() { return m_renderPass3D; }
		const std::vector<VkImage>& GetSwapChainImages() { return m_swapChainImages; }
		VkFormat GetSwapChainImageFormat() { return m_swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return m_swapChainExtent; }

		size_t ImageCount() { return m_swapChainImages.size(); }
		uint32_t Width() { return m_swapChainExtent.width; }
		uint32_t Height() { return m_swapChainExtent.height; }
		size_t GetCurrentFrame() { return  m_currentFrame; }

		float ExtentAspectRatio()
		{
			return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
		}

		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t* imageIdx);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIdx);

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass2D();
		void CreateFrameBuffers();
		void CreateSyncObjects();

		void CreateRenderPass3D();
		void CreateFrameBuffers3D();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkFramebuffer> m_swapChainFrameBuffers;
		VkRenderPass m_renderPass;

		std::vector<VkFramebuffer> m_swapChainFrameBuffers3D;
		VkRenderPass m_renderPass3D;

		std::vector<VkImage> m_depthImages;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkDeviceMemory> m_depthImageMemorys;

		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		vxlDevice& m_device;
		VkExtent2D m_windowExtent;

		VkSwapchainKHR m_swapChain;
		std::shared_ptr<vxlSwapChain> m_oldSwapChain;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		std::vector<VkFence> m_imagesInFlight;
		size_t m_currentFrame{ 0 };


	};
}
