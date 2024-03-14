#pragma once

#include "GP2CommandBuffer.h"
#include "vulkanbase/VulkanUtil.h"

//Pools Create & Own CommandBuffers
class GP2CommandPool
{
public:
	GP2CommandPool() = default;

	void Initialize(const VkDevice& device, const QueueFamilyIndices& queue);
	void Destroy();

	GP2CommandBuffer createCommandBuffer() const;
	VkCommandPool GetVkCommandPool() const { return m_commandPool; }
	QueueFamilyIndices GetQueueFamiliyIndices() const { return m_queueFamilyIndices; }


private:
	VkCommandPool m_commandPool;
	QueueFamilyIndices m_queueFamilyIndices;
	VkDevice m_device;


};
