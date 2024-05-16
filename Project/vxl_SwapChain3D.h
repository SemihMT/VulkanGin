#pragma once
#include "vxl_Device.h"
#include "vulkan/vulkan.h"
#include "vxl_Utils.h"

#include <string>
#include <vector>
#include <memory>

namespace vxl
{
	class vxlSwapChain3D
	{
	public:
		vxlSwapChain3D(vxlDevice& device, VkExtent2D windowExtent);
		vxlSwapChain3D(vxlDevice& device, VkExtent2D windowExtent, std::shared_ptr<vxlSwapChain3D> previous);
		~vxlSwapChain3D();

		vxlSwapChain3D(const vxlSwapChain3D& other) = delete;
		vxlSwapChain3D(vxlSwapChain3D&& other) = delete;
		vxlSwapChain3D& operator=(const vxlSwapChain3D& other) = delete;
		vxlSwapChain3D& operator=(vxlSwapChain3D&& other) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return m_swapChainFrameBuffers[index]; }
		VkImageView GetImageView(int index) { return m_swapChainImageViews[index]; }
		VkRenderPass GetRenderPass() { return m_renderPass; }
		VkFormat GetSwapChainImageFormat() { return m_swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return m_swapChainExtent; }

		size_t ImageCount() { return m_swapChainImages.size(); }
		uint32_t Width() { return m_swapChainExtent.width; }
		uint32_t Height() { return m_swapChainExtent.height; }

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
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObjects();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkFramebuffer> m_swapChainFrameBuffers;
		VkRenderPass m_renderPass;

		std::vector<VkImage> m_depthImages;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkDeviceMemory> m_depthImageMemorys;

		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		vxlDevice& m_device;
		VkExtent2D m_windowExtent;

		VkSwapchainKHR m_swapChain;
		std::shared_ptr<vxlSwapChain3D> m_oldSwapChain;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		std::vector<VkFence> m_imagesInFlight;
		size_t currentFrame{ 0 };


	};
}
