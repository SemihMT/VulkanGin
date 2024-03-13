#pragma once
#include "vulkanbase/VulkanUtil.h"
class GP2VertexBuffer
{
public:
	GP2VertexBuffer() = default;
	~GP2VertexBuffer() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice);
	void CreateVertexBuffer();
	void FillVertexBuffer();
	void Destroy();

	

	VkBuffer GetVertexBuffer() const { return m_vertexBuffer; }
private:

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
	VkBuffer m_vertexBuffer{};
	VkDeviceMemory m_vertexBufferMemory{};

};
