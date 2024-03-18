#pragma once
#include "GP2Buffer.h"
#include "vulkanbase/VulkanUtil.h"
class GP2IndexBuffer
{
public:
	GP2IndexBuffer() = default;
	~GP2IndexBuffer() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool);
	void CreateIndexBuffer(const std::vector<uint16_t>& indices);
	void Destroy();

	GP2Buffer GetBuffer() const { return m_buffer; }

	
private:

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


	VkDevice m_device{};
	VkPhysicalDevice m_physicalDevice{};
	GP2CommandPool m_commandPool{};
	GP2Buffer m_buffer{};

};
