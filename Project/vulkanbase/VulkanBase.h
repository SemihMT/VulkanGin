#ifndef VULKANBASE_H
#define VULKANBASE_H

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
#include "GP2GraphicsPipeline2D.h"
#include "GP2IndexBuffer.h"
#include "GP2Mesh.h"
#include "GP2Shader.h"
#include "GP2VertexBuffer.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};




////Temporary vertex buffer
//const std::vector<Vertex2D> vertices = {
//	   {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
//	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
//	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
//	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
//
//	{{-0.5f + 0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
//	{{0.5f + 0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//	{{0.5f + 0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//	{{-0.5f + 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
//
//};
//
//const std::vector<uint16_t> indices = {
//	0, 1, 2, 2, 3, 0,
//	4, 5, 6, 6, 7, 4
//};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:

	void CreateTempMeshes()
	{

		float xOffset = 0.0f;
		float yOffset = 0.0f;


		xOffset = -0.5f;
		yOffset = -0.5f;
		//Rectangle
		meshes2D.emplace_back(GP2Mesh<Vertex2D>
		{
			device, physicalDevice, m_commandPool,
			{
				{{-0.4f + xOffset, -0.4f + yOffset}, {1.0f, 0.0f, 0.0f}},
				{{0.4f + xOffset, -0.4f + yOffset}, {0.0f, 1.0f, 0.0f}},
				{{0.4f + xOffset, 0.4f + yOffset}, {0.0f, 0.0f, 1.0f}},
				{{-0.4f + xOffset, 0.4f + yOffset}, {1.0f, 1.0f, 1.0f}}
			},
			{
				0, 1, 2, 2, 3, 0
			}
		});

		xOffset = 0.5f;
		yOffset = -0.5f;
		//Hexagon
		meshes2D.emplace_back(GP2Mesh<Vertex2D>
		{
			device, physicalDevice, m_commandPool,
			{
				// Center vertex
				{{ 0.0f + xOffset,  0.0f + yOffset}, {1.0f, 1.0f, 1.0f}},  // Vertex2D 0 (center)
				// Vertices forming hexagon around the center
				{{ 0.0f + xOffset,  0.5f + yOffset}, {1.0f, 0.0f, 0.0f}},  // Vertex2D 1 (top)
				{{ 0.433f + xOffset,  0.25f + yOffset}, {0.0f, 1.0f, 0.0f}}, // Vertex2D 2 (top-right)
				{{ 0.433f + xOffset, -0.25f + yOffset}, {0.0f, 0.0f, 1.0f}}, // Vertex2D 3 (bottom-right)
				{{ 0.0f + xOffset, -0.5f + yOffset}, {1.0f, 1.0f, 1.0f}},    // Vertex2D 4 (bottom)
				{{ -0.433f + xOffset, -0.25f + yOffset}, {1.0f, 1.0f, 1.0f}},// Vertex2D 5 (bottom-left)
				{{ -0.433f + xOffset,  0.25f + yOffset}, {1.0f, 1.0f, 1.0f}} // Vertex2D 6 (top-left)
			},
			{
				// Indices forming a triangle fan
				0, 1, 6, // Triangle 1 (center to top-left)
				0, 6, 5, // Triangle 2 (center to bottom-left)
				0, 5, 4, // Triangle 3 (center to bottom)
				0, 4, 3, // Triangle 4 (center to bottom-right)
				0, 3, 2, // Triangle 5 (center to top-right)
				0, 2, 1  // Triangle 6 (center to top)
			}
		});
		xOffset = 0.0f;
		yOffset = 0.5f;
		//5 pointed star
		meshes2D.emplace_back(GP2Mesh<Vertex2D>
		{
			device, physicalDevice, m_commandPool,
			{
				// Center vertex
				{{ 0.0f + xOffset,  0.0f + yOffset}, {0.9f, 0.9f, 0.9f}},  // Vertex2D 0 (center)
				// Outer vertices of the star
				{{ 0.0f + xOffset,  0.5f + yOffset}, {0.85f, 0.25f, 0.25f}},  // Vertex2D 1 (top)
				{{ 0.1f + xOffset,  0.2f + yOffset}, {0.25f, 0.85f, 0.25f}},  // Vertex2D 2 (top-right)
				{{ 0.5f + xOffset,  0.2f + yOffset}, {0.25f, 0.25f, 0.85f}},  // Vertex2D 3 (top-right extended)
				{{ 0.2f + xOffset, -0.1f + yOffset}, {0.85f, 0.85f, 0.25f}},  // Vertex2D 4 (middle-right)
				{{ 0.3f + xOffset, -0.5f + yOffset}, {0.25f, 0.85f, 0.85f}},  // Vertex2D 5 (bottom-right)
				{{ 0.0f + xOffset, -0.25f + yOffset}, {0.85f, 0.25f, 0.85f}}, // Vertex2D 6 (bottom)
				{{-0.3f + xOffset, -0.5f + yOffset}, {0.85f, 0.85f, 0.25f}},  // Vertex2D 7 (bottom-left)
				{{-0.2f + xOffset, -0.1f + yOffset}, {0.25f, 0.85f, 0.85f}},  // Vertex2D 8 (middle-left)
				{{-0.5f + xOffset,  0.2f + yOffset}, {0.25f, 0.25f, 0.85f}},  // Vertex2D 9 (top-left extended)
				{{-0.1f + xOffset,  0.2f + yOffset}, {0.85f, 0.25f, 0.25f}}   // Vertex2D 10 (top-left)
			},
			{
				// Indices forming triangles for the star (wound in counter-clockwise order)
				0, 2, 1,  // Triangle 1 (center to top-right to top)
				0, 3, 2,  // Triangle 2 (center to top-right extended to top-right)
				0, 4, 3,  // Triangle 3 (center to middle-right to top-right extended)
				0, 5, 4,  // Triangle 4 (center to bottom-right to middle-right)
				0, 6, 5,  // Triangle 5 (center to bottom to bottom-right)
				0, 7, 6,  // Triangle 6 (center to bottom-left to bottom)
				0, 8, 7,  // Triangle 7 (center to middle-left to bottom-left)
				0, 9, 8,  // Triangle 8 (center to top-left extended to middle-left)
				0, 10, 9, // Triangle 9 (center to top-left to top-left extended)
				0, 1, 10  // Triangle 10 (center to top to top-left)
			}
		});
		xOffset = 0.3f;
		yOffset = 0.8f;
		meshes3D.emplace_back(GP2Mesh<Vertex3D>
		{
			device, physicalDevice, m_commandPool,
			{
				// Vertices of the cube
				{{-0.5f + xOffset, -0.5f + yOffset, 0.5f}, {1.0f, 0.0f, 0.0f}}, // Vertex 0 (front-bottom-left)
				{{ 0.5f + xOffset, -0.5f + yOffset, 0.5f}, {0.0f, 1.0f, 0.0f}}, // Vertex 1 (front-bottom-right)
				{{ 0.5f + xOffset,  0.5f + yOffset, 0.5f}, {0.0f, 0.0f, 1.0f}}, // Vertex 2 (front-top-right)
				{{-0.5f + xOffset,  0.5f + yOffset, 0.5f}, {1.0f, 1.0f, 0.0f}}, // Vertex 3 (front-top-left)
				{{-0.5f + xOffset, -0.5f + yOffset,-0.5f}, {0.0f, 1.0f, 1.0f}}, // Vertex 4 (back-bottom-left)
				{{ 0.5f + xOffset, -0.5f + yOffset,-0.5f}, {1.0f, 0.0f, 1.0f}}, // Vertex 5 (back-bottom-right)
				{{ 0.5f + xOffset,  0.5f + yOffset,-0.5f}, {0.5f, 0.5f, 0.5f}}, // Vertex 6 (back-top-right)
				{{-0.5f + xOffset,  0.5f + yOffset,-0.5f}, {0.5f, 0.5f, 0.5f}}  // Vertex 7 (back-top-left)
			},
	{
			// Indices forming triangles for the cube
			0, 1, 2, // Front face
			0, 2, 3,
			1, 5, 6, // Right face
			1, 6, 2,
			4, 7, 6, // Back face
			4, 6, 5,
			4, 0, 3, // Left face
			4, 3, 7,
			3, 2, 6, // Top face
			3, 6, 7,
			4, 5, 1, // Bottom face
			4, 1, 0
		}
		});


	}

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
		// The swapchain is the infrastructure that will own the buffers we render to before presenting them to the screen
		// It's essentially a queue of images, waiting to be presented to the screen
		// The application acquires one of these images, draws to it and returns it to the queue

		// 1. Check if the GPU supports swapchains 'querySwapChainSupport'
		// 2. Store the details of the swapchains we support and store them in 'SwapChainSupportDetails'
		// 3. Choose the right settings for the swapchain:
		//  3a. Choose the Surface Format (32bit/pixel + SRGB-nonlinear)
		//  3b. Choose the Presentation Mode: (prefer MAILBOX and then FIFO)
		//    * The presentation mode is arguably the most important setting for the swapchain
		//    * It defines the actual conditions for showing the images to the screen (when and how)
		// 4. Choose Swap Extent: The resolution of the swap chain images/buffers
		//    * This is almost always the physical screen's resolution
		// 5. Creating the Swap Chain:
		//  5a. Store the surface format, present mode and extent
		//  5b. Specify the minimum number of images we want in memory (minimum supported images + 1)
		//  5c. Specify the maximum number of images (setting this to 0 means there is no max)
		//  5d. FIll in the swapchain create info struct
		//  5e. Set up how the images will be shared between queue families
		//    * If the graphics and present queue are the same: use exclusive ownership
		//	  * else: use share them between the queues
		//  5f. Store handles to the images in 'swapChainImages' for easy retrieval and later use
		//  5g. Store the image format & extent for later use
		createSwapChain();


		// createImageViews:
		// Image views 'VkImageView' are necessary to use any image 'VkImage' in Vulkan
		// An Image View describes how to access the image and which part of the image to access.
		// 1. Resize the 'swapChainImageViews' vector to be the same size as the swapChainImages vector
		// 2. Loop over all swapChainImages
		// 3. Create the CreateInfo struct for the ImageViews
		//  3a. Set the viewType to 2D & the format to be the same as the swapchain images
		//  3b. Set the RGBA channels to default using VK_COMPONENT_SWIZZLE_IDENTITY
		//  3c. Set the subresourceRange field (describes the image's purpose) to use the images as color targets with no mipmapping
		// 4. Create the VkImageView & store it
		// *** The image views are resources that we have to explicitly destroy ourselves ***
		createImageViews();

		// week 03




		// GP2Shader:
		// This class handles the creation of shaders and thus also shader modules
		// The constructor takes in paths to fragment and vertex shaders
		// 1. Initialize the class
		//    * Calls the Create - Fragment / Vertex2D - ShaderInfo Function
		// 2. Creates Vertex2D / Fragment Shader Info
		//  2a. Reads the file at the given path in the constructor & stores the bytes
		//  2b. Creates a VkShaderModule that stores the code + size
		//  2c. Stores the module in the ShaderInfo along with a string representation of the entry point of the shader
		m_GradientShader.initialize(device);
		m_FlatShadingShader.initialize(device);

		//createRenderPass:
		// Sets up the colorAttachment (Render Target) - The Image in memory that vulkan will render to
		// Sets the layout of the render target to be optimal to render into
		// Sets up the subpass that will render into that attachment with the fragment shader
		// Creates the renderPass object

		createRenderPass();



		//Create a GP2GraphicsPipeline Class:
		// It should:
		//	- 

		// createGraphicsPipeline:
		// 1. Define the viewportstate
		// 2. Create the rasterizer
		// 3. Define how to multi-sample
		// 4. Set up color blending - none for now
		// 5. Define dynamic state so we can e.g resize the window without having to recreate the pipeline
		// 6. Create the pipeline layout 'pipelineLayout' that will store shader uniforms like the camera transform
		// 7. Create the Pipeline 'graphicsPipeline'

		m_graphicsPipeline2D.Initialize(device, &m_GradientShader, renderPass);
		m_graphicsPipeline3D.Initialize(device, &m_FlatShadingShader, renderPass);
		//createGraphicsPipeline();

		// createFrameBuffers:
		// This function creates a frame buffer object for each image we have in the swapchain.
		// These are the link between the attachments and the real images in memory
		createFrameBuffers();
		// week 02

		m_commandPool.Initialize(device, findQueueFamilies(physicalDevice));
		m_commandBuffer = m_commandPool.createCommandBuffer();

		/*m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(indices);*/

		CreateTempMeshes();

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

		for (auto& m : meshes2D)
			m.Destroy();
		for (auto& m : meshes3D)
			m.Destroy();

		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_commandPool.Destroy();
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		//vkDestroyPipeline(device, graphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		m_graphicsPipeline2D.Destroy();
		m_graphicsPipeline3D.Destroy();
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

	GP2Shader<Vertex2D> m_GradientShader{ "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
	GP2Shader<Vertex3D> m_FlatShadingShader{ "shaders/flatShading.vert.spv", "shaders/flatShading.frag.spv" };

	//Store different meshes
	std::vector<GP2Mesh<Vertex2D>> meshes2D{ };
	std::vector<GP2Mesh<Vertex3D>> meshes3D{ };


	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();
	void drawScene2D();
	void drawScene3D();
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
	void recordCommandBuffer(GP2CommandBuffer commandBuffer, uint32_t imageIndex);

	// Week 03
	// Renderpass concept
	// Graphics pipeline

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;
	//VkPipeline graphicsPipeline;
	GP2GraphicsPipeline2D<Vertex2D> m_graphicsPipeline2D;
	GP2GraphicsPipeline2D<Vertex3D> m_graphicsPipeline3D;
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
#endif // VULKANBASE_H