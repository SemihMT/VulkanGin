#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "vxl_Device.h"
#include "vxl_SwapChain.h"

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
		void UpdateUBO(uint32_t currentImage, VkExtent2D extent);

	private:

		void CreateDescriptorSetLayout();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		vxlDevice& m_device;

		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;

	};
}
