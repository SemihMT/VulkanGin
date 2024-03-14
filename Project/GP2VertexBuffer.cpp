#include "GP2VertexBuffer.h"
#include "vulkanbase/VulkanBase.h"
void GP2VertexBuffer::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool)
{
	m_device = device;
	m_physicalDevice = physicalDevice;
	m_commandPool = commandPool;
	m_buffer.Initialize(m_device, m_physicalDevice, m_commandPool);
	
}

void GP2VertexBuffer::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	//Create the staging buffer
	GP2Buffer stagingBuffer{};
	stagingBuffer.Initialize(m_device, m_physicalDevice, m_commandPool);
	stagingBuffer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	//Copy data to the staging buffer
	void* data;
	vkMapMemory(m_device, stagingBuffer.GetVkDeviceMemory(), 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device, stagingBuffer.GetVkDeviceMemory());

	//Create the GPU buffer
	m_buffer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//Copy the staging buffer to the GPU
	stagingBuffer.CopyBuffer(m_buffer, bufferSize);

	//Destroy the staging buffer
	stagingBuffer.Destroy();

	
	

	
}

void GP2VertexBuffer::Destroy()
{
	m_buffer.Destroy();
}

uint32_t GP2VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties{};
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

	//Loop over all the memory types
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		//Check if 
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
