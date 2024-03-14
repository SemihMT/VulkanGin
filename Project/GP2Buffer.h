#pragma once
#include "GP2CommandPool.h"
#include "vulkanbase/VulkanUtil.h"

class GP2Buffer
{
public:
	GP2Buffer() = default;
	~GP2Buffer() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void Destroy();
	void CopyBuffer(GP2Buffer dstBuffer, VkDeviceSize size);

	VkBuffer GetVkBuffer() const { return m_buffer; }
	VkDeviceMemory GetVkDeviceMemory() const { return m_bufferMemory; }
private:
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	GP2CommandPool m_commandPool;

	VkBuffer m_buffer{};
	VkDeviceMemory m_bufferMemory{};
};
