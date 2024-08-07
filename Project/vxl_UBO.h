#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "vxl_Device.h"
#include "vxl_SwapChain.h"

namespace vxl
{
	class vxlCamera;
}

namespace vxl
{
	class vxlUBO
	{
	public:
		//Uniform Buffer Object layout:
		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec2 screen;
		};
	public:
		vxlUBO(vxlDevice& device);
		~vxlUBO();

		vxlUBO(const vxlUBO& other) = delete;
		vxlUBO(vxlUBO&& other) = delete;
		vxlUBO& operator=(const vxlUBO& other) = delete;
		vxlUBO& operator=(vxlUBO&& other) = delete;


		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_descriptorSetLayout; }
		void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t currentFrame);
		void UpdateUBO(uint32_t currentImage, VkExtent2D extent, const vxlCamera& camera);

	private:
		// Creating textures alongside the UBO because sending the sampler information to the shaders uses related stuff
		void CreateTextureImage(const std::string& texturePath, VkImage& textureImage, VkDeviceMemory& textureMemory);
		void CreateTextureImageView(VkImageView& textureImageView, const VkImage& textureImage);
		void CreateTextureSampler();

		void CreateDescriptorSetLayout();	// Specifies the types of resources that are going to be accessed by the pipeline
		void CreateUniformBuffers();		// Buffer that is made accessible in a read-only fashion to shaders so that the shaders can read constant parameter data
		void CreateDescriptorPool();		// Create the Descriptor pool so we can acquire descriptor sets
		void CreateDescriptorSets();		// Descriptor sets can't be created directly, they must be allocated from a pool like command buffers


		vxlDevice& m_device;

		VkImage m_textureAtlasImage;
		VkDeviceMemory m_textureAtlasImageMemory;
		VkImageView m_textureAtlasImageView;

		VkImage m_hotbarTextureImage;
		VkDeviceMemory m_hotbarTextureImageMemory;
		VkImageView m_hotbarTextureImageView;

		VkImage m_crosshairTextureImage;
		VkDeviceMemory m_crosshairTextureImageMemory;
		VkImageView m_crosshairTextureImageView;

		VkSampler m_textureSampler;

		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;

	};
}
