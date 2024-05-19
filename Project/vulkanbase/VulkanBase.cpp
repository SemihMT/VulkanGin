#include "vulkanbase/VulkanBase.h"


void VulkanBase::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

void VulkanBase::CreateTempMeshes()
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

void VulkanBase::InitVulkan()
{


	// week 06
	// createInstance:
	// Creates the 'Vulkan Instance'; the connection between the application and the Vulkan library.
	// 1. Creates a 'VkApplicationInfo' struct - this stores extra information about the program that might help the driver optimize
	// 2. Creates a 'VkInstanceCreateInfo' struct - tells the driver which global extensions and V.layers we want to use (Global = applicable for the entire program, not a specific device)
	// 3. Finally Creates the VkInstance object using VkCreateInstance & our CreateInfo struct
	// The instance will need to be destroyed when quitting the program

	CreateInstance();

	// setupDebugMessenger: Does validation layer stuff I don't really want to get into right now :p 
	//

	SetupDebugMessenger();

	// createSurface:
	// Uses GLFW's CreateWindowSurface call to fill a 'VkSurfaceKHR' object
	// This object represents an abstract type of surface that we can render to
	// Which then can be displayed inside a window
	// The Surface will need to be destroyed when quitting the program

	// To be able to use the surface, we need a queue for both drawing and presenting to the surface
	// These can be two different queues or they can be the same, depends on the GPU

	CreateSurface();

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

	PickPhysicalDevice();


	// createLogicalDevice:
	// Creates the 'VkDevice' object that represents the GPU with our preferred features enabled/disabled
	// Also specifies how many queues of each available queueFamily we want this device to access
	// For this project, 1 graphics queue and 1 present queue is created and usable by the VkDevice
	CreateLogicalDevice();

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
	CreateSwapChain();


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
	CreateImageViews();

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

	CreateRenderPass();

	//layout: 
	CreateDescriptorSetLayout();
	//buffer:
	CreateUniformBuffers();
	//pool:
	CreateDescriptorPool();
	//set:
	CreateDescriptorSets();

	CreatePushConstantRanges();

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
	CreateFrameBuffers();
	// week 02

	m_commandPool.Initialize(device, FindQueueFamilies(physicalDevice));
	m_commandBuffer = m_commandPool.createCommandBuffer();

	//CreateDepthResources();

	/*m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(indices);*/

	CreateTempMeshes();

	// week 06
	CreateSyncObjects();
}

void VulkanBase::CalculateDeltaTime()
{
	// Calculate delta time
	std::chrono::steady_clock::time_point currentFrameTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentFrameTime - m_lastFrameTime);
	m_deltaTime = timeElapsed.count();

	// Update last frame time for the next iteration
	m_lastFrameTime = currentFrameTime;
}

void VulkanBase::Update()
{
	CalculateDeltaTime();

	ProcessInput(window);
}

void VulkanBase::MainLoop()
{
	m_lastFrameTime = std::chrono::steady_clock::now(); // Initialize last frame time
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Update();
		// week 06
		DrawFrame();
	}
	vkDeviceWaitIdle(device);
}

void VulkanBase::Cleanup()
{

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

	if (g_enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroySwapchainKHR(device, swapChain, nullptr);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

VkFormat VulkanBase::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
	VkFormatFeatureFlags features)
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

VkFormat VulkanBase::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool VulkanBase::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanBase::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void VulkanBase::MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	
}

void VulkanBase::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	
}

void VulkanBase::ProcessInput(GLFWwindow* window)
{

}

void VulkanBase::InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);


	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void VulkanBase::DrawScene2D() {
	//Replace with actual scene drawing code
	//foreach mesh in m_Scenes.current -> draw
	//vkCmdDrawIndexed(m_commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
	for (const auto& mesh : meshes2D)
	{
		mesh.Bind(m_commandBuffer);
		mesh.Draw(m_commandBuffer);
	}
}

void VulkanBase::DrawScene3D() {
	//Replace with actual scene drawing code
	//foreach mesh in m_Scenes.current -> draw
	//vkCmdDrawIndexed(m_commandBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
	for (const auto& mesh : meshes3D)
	{
		mesh.Bind(m_commandBuffer);
		mesh.Draw(m_commandBuffer);
	}
}



void VulkanBase::RecordCommandBuffer(GP2CommandBuffer commandBuffer, uint32_t imageIndex) {
	DrawFrame(imageIndex);

}

void VulkanBase::DrawFrame(uint32_t imageIndex) {

	//This function should get factorized into each separate
	//element's setup or something

	//binds the renderpass to the framebuffer we can render to
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	auto commandBuffer = m_commandBuffer.GetVkCommandBuffer();

	//Describes to what portion of the framebuffer we render
	//Will almost always be 0,0 to window width, height
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	//The scissor filter that discards any pixels that is not inside the extent it defines
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//Probably not the most efficient way of doing things...
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline2D.GetVkGraphicsPipeline());
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline2D.GetVkPipelineLayout(), 0, 1, &descriptorSets[imageIndex], 0, nullptr);
	auto constants = UpdatePushConstants();
	vkCmdPushConstants(commandBuffer, m_graphicsPipeline2D.GetVkPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &constants);
	DrawScene2D();

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline3D.GetVkGraphicsPipeline());
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline3D.GetVkPipelineLayout(), 0, 1, &descriptorSets[imageIndex], 0, nullptr);
	vkCmdPushConstants(commandBuffer, m_graphicsPipeline3D.GetVkPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &constants);

	DrawScene3D();
	vkCmdEndRenderPass(commandBuffer);
}

QueueFamilyIndices VulkanBase::FindQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.IsComplete()) {
			break;
		}

		i++;
	}

	return indices;
}


void VulkanBase::CreateFrameBuffers() {
	//Resize the vector that holds the framebuffers
	//we need one for each image in our swapchain
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}



void VulkanBase::CreateRenderPass() {
	//Color attachment.This is the description of the image we will be writing into with rendering commands.
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat; //rendering the image should be compliant with the swapchain
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

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	//The image life will go something like this:
	//UNDEFINED->RenderPass Begins->Subpass 0 begins(Transition to Attachment Optimal)
	//->Subpass 0 renders->Subpass 0 ends->Renderpass Ends(Transitions to Present Source)

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanBase::CreateUniformBuffers()
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

void VulkanBase::CreateDescriptorSetLayout()
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

void VulkanBase::CreateDescriptorPool()
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

void VulkanBase::CreateDescriptorSets()
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

void VulkanBase::UpdateUniformBuffer(uint32_t currentImage) const
{

	UniformBufferObject ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::mat4(1.0);
	//ubo.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	//ubo.proj = glm::perspective(glm::radians(fov), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
	ubo.proj[1][1] *= -1;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void VulkanBase::CreatePushConstantRanges()
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

PushConstant VulkanBase::UpdatePushConstants()
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

//
//void VulkanBase::createGraphicsPipeline() {
//
//	//Let the pipeline know how many viewports and scissors we'll use
//	//This can be done without recreating the pipeline because we use dynamic state, see below
//	VkPipelineViewportStateCreateInfo viewportState{};
//	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	viewportState.viewportCount = 1;
//	viewportState.scissorCount = 1;
//
//	//The rasterizer, Creates the fragments from the geometry created by the vert shader
//	VkPipelineRasterizationStateCreateInfo rasterizer{};
//	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	rasterizer.depthClampEnable = VK_FALSE;
//	rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//	rasterizer.lineWidth = 1.0f;
//	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//	rasterizer.depthBiasEnable = VK_FALSE;
//
//	//Multisampling for anti-aliasing
//	//Disabled for now
//	VkPipelineMultisampleStateCreateInfo multisampling{};
//	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	multisampling.sampleShadingEnable = VK_FALSE;
//	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//	//Setup color blending between the frambuffer color and the fragment colors returned by the rasterizer
//	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable = VK_FALSE; //Not blending any colors
//	/*FOR ALPHA BLENDING:
//	 *colorBlendAttachment.blendEnable = VK_TRUE;
//	 *colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//	 *colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//	 *colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
//	 *colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//	 *colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//	 *colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
//	 */
//
//	VkPipelineColorBlendStateCreateInfo colorBlending{};
//	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	colorBlending.logicOpEnable = VK_FALSE;
//	colorBlending.logicOp = VK_LOGIC_OP_COPY;
//	colorBlending.attachmentCount = 1;
//	colorBlending.pAttachments = &colorBlendAttachment;
//	colorBlending.blendConstants[0] = 0.0f;
//	colorBlending.blendConstants[1] = 0.0f;
//	colorBlending.blendConstants[2] = 0.0f;
//	colorBlending.blendConstants[3] = 0.0f;
//
//
//	//Creating Dynamic State:
//	//The following specifies that we want to leave out
//	//Size of the viewport, line width and blend constants
//	//As these can be modified at runtime without recreating the pipeline
//	std::vector<VkDynamicState> dynamicStates = {
//		VK_DYNAMIC_STATE_VIEWPORT,
//		VK_DYNAMIC_STATE_SCISSOR
//	};
//	VkPipelineDynamicStateCreateInfo dynamicState{};
//	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//	dynamicState.pDynamicStates = dynamicStates.data();
//
//	//Pipeline layout - important for uniform values for shaders
//	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo.setLayoutCount = 0;
//	pipelineLayoutInfo.pushConstantRangeCount = 0;
//
//	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create pipeline layout!");
//	}
//
//	VkGraphicsPipelineCreateInfo pipelineInfo{};
//
//	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//
//	/*VkPipelineShaderStageCreateInfo vertShaderStageInfo = createVertexShaderInfo();
//	VkPipelineShaderStageCreateInfo fragShaderStageInfo = createFragmentShaderInfo();
//
//	VkPipelineShaderStageCreateInfo shaderStages[] = {
//		vertShaderStageInfo,
//		fragShaderStageInfo
//	};*/
//
//	pipelineInfo.stageCount = 2;
//	pipelineInfo.pStages = m_GradientShader.getShaderStages().data();
//
//	//Describing Vertex2D Input info in the Shader class we made
//	//Because we have no vertex buffer, there is no actual vertex data here
//	auto vertexInputState = m_GradientShader.createVertexInputStateInfo(device);
//	//Describing how we assemble the vertices we get (Triangle List)
//	auto inputAssemblyState = m_GradientShader.createInputAssemblyStateInfo(device);
//
//	pipelineInfo.pVertexInputState = &vertexInputState;
//	pipelineInfo.pInputAssemblyState = &inputAssemblyState;
//
//	pipelineInfo.pViewportState = &viewportState;
//	pipelineInfo.pRasterizationState = &rasterizer;
//	pipelineInfo.pMultisampleState = &multisampling;
//	pipelineInfo.pColorBlendState = &colorBlending;
//	pipelineInfo.pDynamicState = &dynamicState;
//	pipelineInfo.layout = pipelineLayout;
//	pipelineInfo.renderPass = renderPass;
//	pipelineInfo.subpass = 0;
//	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//
//	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create graphics pipeline!");
//	}
//
//	m_GradientShader.destroyShaderModules(device);
//}


SwapChainSupportDetails VulkanBase::QuerySwapChainSupport(VkPhysicalDevice device) {

	//Fill in the VkSurfaceCapabilitiesKHR struct inside SwapChainSupportDetails
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//Get the pixel formats this GPU supports
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//Get the available presentation modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

//Function that will choose the appropriate surface format
//For us that is preferably one that stores 32 bits per pixel in SRGB color space
VkSurfaceFormatKHR VulkanBase::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanBase::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanBase::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void VulkanBase::CreateSwapChain() {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void VulkanBase::CreateImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}


void VulkanBase::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices{ deviceCount };
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice device) {

	QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	//If there is a queue for drawing & presenting + all the extensions we need are supported : DEVICE IS SUITABLE
	return indices.IsComplete() && extensionsSupported;

}

void VulkanBase::CreateLogicalDevice() {
	//Find the queueFamilies we need (graphics & present)
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// Define queue creation info
	// Most importantly, we describe the number of queues (queueCount) we want of each QueueFamily

	float queuePriority = 1.0f; //Priority between queues of the same family, for command buffer scheduling reasons
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	//Specifying which features we want to enable in this logical VkDevice
	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (g_enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	//Getting a handle to the Queues that are assigned to this device
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}


void VulkanBase::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{

	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	//Specify the severity we want the callback to be called for
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	//Specify which types of messages the callback is notified about
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = DebugCallback;
}


void VulkanBase::SetupDebugMessenger() {
	if (!g_enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VulkanBase::CreateSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}

}

void VulkanBase::DrawFrame() {
	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &inFlightFence);

	//Gets the next image we can render to
	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	//Reset the cmdBuffer - begin of frame
	m_commandBuffer.Reset();
	m_commandBuffer.BeginRecording(); //Record to the command buffer


	RecordCommandBuffer(m_commandBuffer, imageIndex); // calls drawFrame

	UpdateUniformBuffer(imageIndex);


	m_commandBuffer.EndRecording();


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	m_commandBuffer.Submit(submitInfo);

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);
}

VkBool32 VulkanBase::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}


// Validation Layer Support:
bool CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////



// Extension Support:

std::vector<const char*> VulkanBase::GetRequiredExtensions()
{
	//The GLFW Vulkan extensions are required because we use that for window and surface support
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//If we want to have a custom callback for debug messages, we need the following extension
	if (g_enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice device) {

	//Get # of extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	//Get and store the extensions themselves
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());


	//Print the available extensions
	std::cout << "available extensions:\n";
	for (const auto& extension : availableExtensions) {
		std::cout << '\t' << extension.extensionName << '\n';
	}

	//Create a unique buffer of the required extensions (user defined?) names
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	//Erase the available extensions from the required ones
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	//if the required ext. list is now empty, our device supports all the extensions we need so TRUE
	//else FALSE
	return requiredExtensions.empty();
}
////////////////////////////////////////////////////////////////////

// Create the VkInstance
void VulkanBase::CreateInstance()
{
	// If we want validation layers but there is no support for them -> ERROR
	if (g_enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	//Technically optional info, but may provide optimizations if the engine is well-known 
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VoxelEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "543Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	//Tell the VkInstance which extensions & validation layers we want to use
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (g_enableValidationLayers) 
	{

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}