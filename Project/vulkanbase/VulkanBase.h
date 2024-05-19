#ifndef VULKANBASE_H
#define VULKANBASE_H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <stb_image.h>

#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "GP2CommandPool.h"
#include "GP2GraphicsPipeline2D.h"
#include "GP2IndexBuffer.h"
#include "GP2Mesh.h"
#include "GP2Shader.h"
#include "GP2VertexBuffer.h"



const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class VulkanBase {
public:
	void Run();

private:

	void CreateTempMeshes();

	GLFWwindow* window;
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void Cleanup();

	std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTime{};
	double m_deltaTime{};
	void CalculateDeltaTime();


	

	void Update();


	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkFormat FindDepthFormat();

	bool HasStencilComponent(VkFormat format);

	void CreateTextureImage() {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("Resources/Textures/statue.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;


	}


	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		//allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	void CreateDepthResources()
	{
		VkFormat depthFormat = FindDepthFormat();

		CreateImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		//depthImageView = CreateImageView(depthImage, depthFormat);
	}


	void CreateSurface();

	GP2Shader<Vertex2D> m_GradientShader{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
	GP2Shader<Vertex3D> m_FlatShadingShader{ "shaders/flatShading.vert.spv", "shaders/flatShading.frag.spv" };

	//Store different meshes
	std::vector<GP2Mesh<Vertex2D>> meshes2D{ };
	std::vector<GP2Mesh<Vertex3D>> meshes3D{ };



	static void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);

	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void ProcessInput(GLFWwindow* window);

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	
	void DrawScene2D();
	void DrawScene3D();

	// Week 02
	// Queue families
	// CommandBuffer concept


	GP2CommandPool m_commandPool;
	GP2CommandBuffer m_commandBuffer;
	//VkCommandPool commandPool;
	//VkCommandBuffer commandBuffer;

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	void DrawFrame(uint32_t imageIndex);
	void RecordCommandBuffer(GP2CommandBuffer commandBuffer, uint32_t imageIndex);

	// Week 03
	// Renderpass concept
	// Graphics pipeline

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//VkPipeline graphicsPipeline;
	GP2GraphicsPipeline2D<Vertex2D> m_graphicsPipeline2D;
	GP2GraphicsPipeline2D<Vertex3D> m_graphicsPipeline3D;
	VkRenderPass renderPass;



	std::vector<GP2Buffer> uniformBuffers;
	std::vector<void*> uniformBuffersMapped;

	void CreateFrameBuffers();
	void CreateRenderPass();

	void CreateUniformBuffers();

	void CreateDescriptorSetLayout();

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void UpdateUniformBuffer(uint32_t currentImage) const;


	VkPushConstantRange m_pushConstantRange;
	void CreatePushConstantRanges();


	PushConstant UpdatePushConstants();
	//void createGraphicsPipeline();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CreateImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void CreateLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	void CreateInstance();

	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupDebugMessenger();
	std::vector<const char*> GetRequiredExtensions();
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	

	void CreateSyncObjects();
	void DrawFrame();

	//debug callback function that's used by vulkan
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};
#endif // VULKANBASE_H