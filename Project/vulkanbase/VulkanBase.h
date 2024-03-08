#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
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

#include "GP2CommandPool.h"
#include "GP2Shader.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() {


		// week 06


		// createInstance:
		// Creates the 'Vulkan Instance'; the connection between the application and the Vulkan library.
		// 1. Creates a 'VkApplicationInfo' struct - this stores extra information about the program that might help the driver optimize
		// 2. Creates a 'VkInstanceCreateInfo' struct - tells the driver which global extensions and V.layers we want to use (Global = applicable for the entire program, not a specific device)
		// 3. Finally Creates the VkInstance object using VkCreateInstance & our CreateInfo struct
		// The instance will need to be destroyed when quitting the program

		createInstance();

		// setupDebugMessenger: Does validation layer stuff I don't really want to get into right now :p 
		//

		setupDebugMessenger();

		// createSurface:
		// Uses GLFW's CreateWindowSurface call to fill a 'VkSurfaceKHR' object
		// This object represents an abstract type of surface that we can render to
		// Which then can be displayed inside a window
		// The Surface will need to be destroyed when quitting the program

		// To be able to use the surface, we need a queue for both drawing and presenting to the surface
		// These can be two different queues or they can be the same, depends on the GPU

		createSurface();

		// week 05


		// pickPhysicalDevice:
		// After Initializing the VkInstance, we need to find and select the graphics card in the system that has the features we need
		// The selected GPU is stored inside a 'VkPhysicalDevice' object
		// This object is destroyed implicitly when the Instance is destroyed, no need for explicit cleanup
		//
		// 1. Finds the number of GPUs available on the system
		// 2. Stores the handles to all the devices in a buffer
		// 3. Checks if the devices are suitable:
		//	3a. Checks if the device has a queueFamily that supports drawing and one that supports presenting
		//	3b. Checks if the required extensions are supported by comparing the available extensions of the device with the user defined 'const std::vector<const char*> deviceExtensions'.

		pickPhysicalDevice();


		// createLogicalDevice:
		// Creates the 'VkDevice' object that represents the GPU with our preferred features enabled/disabled
		// Also specifies how many queues of each available queueFamily we want this device to access
		// For this project, 1 graphics queue and 1 present queue is created and usable by the VkDevice
		createLogicalDevice();

		// week 04

		// createSwapChain:
		// 

		createSwapChain();
		createImageViews();
		
		// week 03
		m_GradientShader.initialize(device);
		createRenderPass();
		createGraphicsPipeline();
		createFrameBuffers();
		// week 02
		m_commandPool.Initialize(device, findQueueFamilies(physicalDevice));
		m_commandBuffer = m_commandPool.createCommandBuffer();
		//createCommandPool();
		//createCommandBuffer();

		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_commandPool.Destroy();
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	

	

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	GP2Shader m_GradientShader{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };


	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();
	void drawScene();

	// Week 02
	// Queue families
	// CommandBuffer concept


	GP2CommandPool m_commandPool;
	GP2CommandBuffer m_commandBuffer;
	//VkCommandPool commandPool;
	//VkCommandBuffer commandBuffer;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void drawFrame(uint32_t imageIndex);
	void createCommandBuffer();
	void createCommandPool(); 
	void recordCommandBuffer(GP2CommandBuffer commandBuffer, uint32_t imageIndex);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;

	void createFrameBuffers();
	void createRenderPass();
	void createGraphicsPipeline();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};