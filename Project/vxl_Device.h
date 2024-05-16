#pragma once

#include "Vxl_Window.h"
#include <string>
#include <vector>
#include <optional>


namespace vxl
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	//This struct stores the index to the queue families that support the types of commands we'll be using for the program
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};


	class vxlDevice
	{
	public:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
	public:
		vxlDevice(vxlWindow& window);
		~vxlDevice();

		vxlDevice(const vxlDevice& other) = delete;
		vxlDevice(vxlDevice&& other) = delete;
		vxlDevice& operator=(const vxlDevice& other) = delete;
		vxlDevice& operator=(vxlDevice&& other) = delete;

		//Resources
		VkCommandPool GetCommandPool() const { return m_commandPool; }
		VkDevice GetDevice() const { return m_device; }
		VkSurfaceKHR GetSurface() const { return m_surface; }
		VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
		VkQueue GetPresentQueue() const { return m_presentQueue; }

		//Query functions
		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_physicalDevice); }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindQueueFamiliesPhysicalDevice();
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		//Buffer helpers
		void CreateBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
		void CleanupBuffer(VkBuffer buffer, VkDeviceMemory bufferMemory);

		//Image helper
		void CreateImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

	private:
		VkPhysicalDeviceProperties m_properties;

		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		// helper functions
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGLFWRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		vxlWindow& m_window;
		VkCommandPool m_commandPool;

		VkDevice m_device;
		VkSurfaceKHR m_surface; //object that represents an abstract type of surface to present rendered images to.
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}
