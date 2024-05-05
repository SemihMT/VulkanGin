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


		glm::vec3 worldPos{};

		worldPos = { 10,0,10 };
		//plane
		meshes3D.emplace_back(GP2Mesh<Vertex3D>
		{
			device, physicalDevice, m_commandPool,
			{
				// Vertices of the plane
				{{-1.5f, -1.5f, 1.5f}, {0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f}}, // Vertex 0 (top-left)
				{{ 1.5f, -1.5f, 1.5f}, {0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f}}, // Vertex 1 (top-right)
				{{ 1.5f, -1.5f,-1.5f}, {0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f}}, // Vertex 2 (bottom-right)
				{{-1.5f, -1.5f,-1.5f}, {0.5f, 0.5f, 0.5f},{0.0f,1.0f,0.0f}}  // Vertex 3 (bottom-left)
			},
	{
		// Indices forming triangles for the plane
		0, 1, 2, // Triangle 1
		2, 3, 0  // Triangle 2
	}
		});


		xOffset = 1.0f;
		yOffset = -1.0f;
		float size = 0.5f;
		meshes3D.emplace_back(GP2Mesh<Vertex3D>
		{
			device, physicalDevice, m_commandPool,
			{
				// Vertices of the cube
				{{-size + xOffset, -size + yOffset, size}, {0.5f,0.5f,0.5f},{0.0f, 0.0f, 0.0f}}, // Vertex 0 (front-bottom-left)
				{{ size + xOffset, -size + yOffset, size}, {0.5f,0.5f,0.5f},{0.0f, 0.0f, 0.0f}}, // Vertex 1 (front-bottom-right)
				{{ size + xOffset,  size + yOffset, size}, {0.5f,0.5f,0.5f},{0.0f, 1.0f, 0.0f}}, // Vertex 2 (front-top-right)
				{{-size + xOffset,  size + yOffset, size}, {0.5f,0.5f,0.5f},{0.0f, 1.0f, 0.0f}}, // Vertex 3 (front-top-left)
				{{-size + xOffset, -size + yOffset,-size}, {0.5f,0.5f,0.5f},{0.0f, 0.0f, 0.0f}}, // Vertex 4 (back-bottom-left)
				{{ size + xOffset, -size + yOffset,-size}, {0.5f,0.5f,0.5f},{0.0f, 0.0f, 0.0f}}, // Vertex 5 (back-bottom-right)
				{{ size + xOffset,  size + yOffset,-size}, {0.5f,0.5f,0.5f},{0.0f, 1.0f, 0.0f}}, // Vertex 6 (back-top-right)
				{{-size + xOffset,  size + yOffset,-size}, {0.5f,0.5f,0.5f},{0.0f, 1.0f, 0.0f}}  // Vertex 7 (back-top-left)
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

		meshes3D.emplace_back(
			device, physicalDevice, m_commandPool, "Resources/Bunny.obj"
		);

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

		//layout: 
		createDescriptorSetLayout();
		//buffer:
		createUniformBuffers();
		//pool:
		createDescriptorPool();
		//set:
		createDescriptorSets();

		createPushConstantRanges();

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

		m_graphicsPipeline2D.Initialize(device, &m_GradientShader, renderPass, descriptorSetLayout, m_pushConstantRange);
		m_graphicsPipeline3D.Initialize(device, &m_FlatShadingShader, renderPass, descriptorSetLayout, m_pushConstantRange);
		//createGraphicsPipeline();

		// createFrameBuffers:
		// This function creates a frame buffer object for each image we have in the swapchain.
		// These are the link between the attachments and the real images in memory
		createFrameBuffers();
		// week 02

		m_commandPool.Initialize(device, findQueueFamilies(physicalDevice));
		m_commandBuffer = m_commandPool.createCommandBuffer();

		//CreateDepthResources();

		/*m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(indices);*/

		CreateTempMeshes();

		// week 06
		createSyncObjects();
	}
	std::chrono::time_point<std::chrono::steady_clock> m_lastFrameTime{};
	double m_deltaTime{};

	void CalculateDeltaTime()
	{
		// Calculate delta time
		std::chrono::steady_clock::time_point currentFrameTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentFrameTime - m_lastFrameTime);
		m_deltaTime = timeElapsed.count();

		// Update last frame time for the next iteration
		m_lastFrameTime = currentFrameTime;
	}


	// camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	inline static glm::vec3 cameraFront{ glm::vec3(0.0f, 0.0f, -1.0f) };
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	inline static bool firstMouse = true;
	inline static float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	inline static float pitch = 0.0f;
	inline static float lastX = WIDTH / 2.0;
	inline static float lastY = HEIGHT / 2.0;
	inline static float fov = 45.0f;

	void update()
	{
		CalculateDeltaTime();

		processInput(window);
	}

	void mainLoop() {
		m_lastFrameTime = std::chrono::steady_clock::now(); // Initialize last frame time
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			update();
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

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			uniformBuffers[i].Destroy();
		}


		vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

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


	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	/*void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
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
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

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
		VkFormat depthFormat = findDepthFormat();

		createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		depthImageView = createImageView(depthImage, depthFormat);
	}*/


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



	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	}
	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * m_deltaTime);
		glm::vec3 newCamPos{};
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos += glm::vec3{ 0,1,0 }*cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraPos -= glm::vec3{ 0,1,0 }*cameraSpeed;

	}

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

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	//VkPipeline graphicsPipeline;
	GP2GraphicsPipeline2D<Vertex2D> m_graphicsPipeline2D;
	GP2GraphicsPipeline2D<Vertex3D> m_graphicsPipeline3D;
	VkRenderPass renderPass;



	std::vector<GP2Buffer> uniformBuffers;
	std::vector<void*> uniformBuffersMapped;

	void createFrameBuffers();
	void createRenderPass();

	void createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			uniformBuffers[i].Initialize(device, physicalDevice, m_commandPool);
			uniformBuffers[i].CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkMapMemory(device, uniformBuffers[i].GetVkDeviceMemory(), 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}
	void createDescriptorPool()
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);


		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i].GetVkBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional
			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}

	}
	void updateUniformBuffer(uint32_t currentImage)
	{

		UniformBufferObject ubo{};
		//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.model = glm::mat4(1.0);
		ubo.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ubo.proj = glm::perspective(glm::radians(fov), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}


	VkPushConstantRange m_pushConstantRange;
	void createPushConstantRanges()
	{
		//setup push constants
		VkPushConstantRange pushConstant;
		//this push constant range starts at the beginning
		pushConstant.offset = 0;
		//this push constant range takes up the size of a MeshPushConstants struct
		pushConstant.size = sizeof(PushConstant);
		//this push constant range is accessible only in the vertex shader
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		m_pushConstantRange = pushConstant;
	}


	PushConstant updatePushConstants()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		// Calculate light direction based on time
		float angle = time * 3.141592f * 2.0f; // Full rotation in 24 hours (assuming time is in seconds)
		float x = 0.0f;
		float y = 1.0f;
		float z = 0.0f; // Assuming negative Z is "up" in your world

		// Handle Z-up world
		glm::vec3 lightDirection = glm::normalize(glm::vec3(x, y, z));

		// Update push constants
		PushConstant constant;
		constant.lightDirection = lightDirection;
		return constant;
	}
	//void createGraphicsPipeline();

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