#include "vxl_SwapChain.h"

#include <array>
#include <iostream>
#include <stdexcept>

namespace vxl
{
	vxlSwapChain::vxlSwapChain(vxlDevice& device, VkExtent2D windowExtent) :
		m_device{ device },
		m_windowExtent{ windowExtent }
	{
		Init();
	}

	vxlSwapChain::vxlSwapChain(vxlDevice& device, VkExtent2D windowExtent, std::shared_ptr<vxlSwapChain> previous) :
		m_device{ device },
		m_windowExtent{ windowExtent },
		m_oldSwapChain{ previous }
	{
		Init();


		m_oldSwapChain = nullptr;
	}

	vxlSwapChain::~vxlSwapChain()
	{
		for (auto imageView : m_swapChainImageViews) {
			vkDestroyImageView(m_device.GetDevice(), imageView, nullptr);
		}
		m_swapChainImageViews.clear();

		if (m_swapChain != nullptr) {
			vkDestroySwapchainKHR(m_device.GetDevice(), m_swapChain, nullptr);
			m_swapChain = nullptr;
		}

		for (int i = 0; i < m_depthImages.size(); i++) {
			vkDestroyImageView(m_device.GetDevice(), m_depthImageViews[i], nullptr);
			vkDestroyImage(m_device.GetDevice(), m_depthImages[i], nullptr);
			vkFreeMemory(m_device.GetDevice(), m_depthImageMemorys[i], nullptr);
		}

		for (auto framebuffer : m_swapChainFrameBuffers) {
			vkDestroyFramebuffer(m_device.GetDevice(), framebuffer, nullptr);
		}
		for (auto framebuffer : m_swapChainFrameBuffers3D) {
			vkDestroyFramebuffer(m_device.GetDevice(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(m_device.GetDevice(), m_renderPass, nullptr);
		vkDestroyRenderPass(m_device.GetDevice(), m_renderPass3D, nullptr);

		// cleanup synchronization objects
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_device.GetDevice(), m_renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_device.GetDevice(), m_imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_device.GetDevice(), m_inFlightFences[i], nullptr);
		}
	}

	void vxlSwapChain::Init()
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass2D();
		CreateRenderPass3D();
		CreateDepthResources();
		CreateFrameBuffers();
		CreateFrameBuffers3D();
		CreateSyncObjects();
	}

	VkFormat vxlSwapChain::FindDepthFormat()
	{
		return m_device.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
						VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkResult vxlSwapChain::AcquireNextImage(uint32_t* imageIdx)
	{
		//Wait until the GPU has finished executing all the commands of the previous frame
		vkWaitForFences(m_device.GetDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		//Ask for the next image index that we can render to
		//The semaphore (sync primitive for GPU internal processes like command queues) should not be signaled, meaning: there should not be any tasks that the GPU still has to execute
		const VkResult result = vkAcquireNextImageKHR(m_device.GetDevice(), m_swapChain, std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, imageIdx);
		return result;
	}

	VkResult vxlSwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIdx)
	{
		if (m_imagesInFlight[*imageIdx] != VK_NULL_HANDLE) {
			vkWaitForFences(m_device.GetDevice(), 1, &m_imagesInFlight[*imageIdx], VK_TRUE, std::numeric_limits<uint64_t>::max());
		}
		m_imagesInFlight[*imageIdx] = m_inFlightFences[m_currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(m_device.GetDevice(), 1, &m_inFlightFences[m_currentFrame]);
		if (vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = imageIdx;

		auto result = vkQueuePresentKHR(m_device.GetPresentQueue(), &presentInfo);
		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result;
	}

	

	void vxlSwapChain::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = m_device.GetSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_device.GetSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = m_device.FindQueueFamiliesPhysicalDevice();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChain;

		if (vkCreateSwapchainKHR(m_device.GetDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		// Remember that we only specified a minimum number of images in the swap chain,
		// so the implementation is allowed to create a swap chain with more.
		// That's why we'll first query the final number of images with vkGetSwapchainImagesKHR,
		// then resize the container and finally call it again to retrieve the handles.
		vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapChain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device.GetDevice(), m_swapChain, &imageCount, m_swapChainImages.data());

		m_swapChainImageFormat = surfaceFormat.format;
		m_swapChainExtent = extent;
	}

	void vxlSwapChain::CreateImageViews()
	{
		m_swapChainImageViews.resize(m_swapChainImages.size());

		for (size_t i = 0; i < m_swapChainImages.size(); i++) {
			m_swapChainImageViews[i] = m_device.CreateImageView(m_swapChainImages[i], m_swapChainImageFormat);
		}
	}

	void vxlSwapChain::CreateDepthResources()
	{
		VkFormat depthFormat = FindDepthFormat();
		VkExtent2D swapChainExtent = GetSwapChainExtent();

		m_depthImages.resize(ImageCount());
		m_depthImageMemorys.resize(ImageCount());
		m_depthImageViews.resize(ImageCount());

		for (int i = 0; i < m_depthImages.size(); i++) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			m_device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_depthImages[i],
				m_depthImageMemorys[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_depthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device.GetDevice(), &viewInfo, nullptr, &m_depthImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void vxlSwapChain::CreateRenderPass2D()
	{
		// Color attachment for the overlay
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = GetSwapChainImageFormat(); // same format as the swapchain
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // load the previous contents
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // start from the present source layout
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // stay in present source layout

		//Depth attachment for depth buffering
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference dummyDepthAttachmentRef{};
		dummyDepthAttachmentRef.attachment = 1;
		dummyDepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &dummyDepthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstSubpass = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device.GetDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void vxlSwapChain::CreateFrameBuffers()
	{
		//Resize the vector that holds the framebuffers
		//we need one for each image in our swapchain
		m_swapChainFrameBuffers.resize(ImageCount());

		for (size_t i = 0; i < ImageCount(); ++i) {
			std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i], m_depthImageViews[i] };

			VkExtent2D swapChainExtent = GetSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device.GetDevice(), &framebufferInfo, nullptr, &m_swapChainFrameBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void vxlSwapChain::CreateSyncObjects()
	{
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_imagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(m_device.GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device.GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device.GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	void vxlSwapChain::CreateRenderPass3D()
	{
		//Color attachment.This is the description of the image we will be writing into with rendering commands.
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = GetSwapChainImageFormat(); //rendering the image should be compliant with the swapchain
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

		//Depth attachment for depth buffering
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstSubpass = 0;
		dependency.dstStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		//The image life will go something like this:
		//UNDEFINED->RenderPass Begins->Subpass 0 begins(Transition to Attachment Optimal)
		//->Subpass 0 renders->Subpass 0 ends->Renderpass Ends(Transitions to Present Source)

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device.GetDevice(), &renderPassInfo, nullptr, &m_renderPass3D) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void vxlSwapChain::CreateFrameBuffers3D()
	{
		//Resize the vector that holds the framebuffers
		//we need one for each image in our swapchain
		m_swapChainFrameBuffers3D.resize(ImageCount());

		for (size_t i = 0; i < ImageCount(); ++i) {
			std::array<VkImageView, 2> attachments = { m_swapChainImageViews[i], m_depthImageViews[i] };

			VkExtent2D swapChainExtent = GetSwapChainExtent();
			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass3D;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device.GetDevice(), &framebufferInfo, nullptr, &m_swapChainFrameBuffers3D[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	VkSurfaceFormatKHR vxlSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR vxlSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		//Prefer Mailbox over FIFO
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				std::cout << "Present mode: Mailbox" << '\n';
				return availablePresentMode;
			}
		}

		//Optionally, we can try immediate
		// for (const auto &availablePresentMode : availablePresentModes) {
		//   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		//     std::cout << "Present mode: Immediate" << std::endl;
		//     return availablePresentMode;
		//   }
		// }

		std::cout << "Present mode: V-Sync" << '\n';
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D vxlSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = m_windowExtent;
			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
}
