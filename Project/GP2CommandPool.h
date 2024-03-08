#pragma once

#include "GP2CommandBuffer.h"

struct QueueFamilyIndices;

//Pools Create & Own CommandBuffers
class GP2CommandPool
{
public:
	GP2CommandPool() = default;

	void Initialize(const VkDevice& device, const QueueFamilyIndices& queue);
	void Destroy();

	GP2CommandBuffer createCommandBuffer() const;

private:
	VkCommandPool m_commandPool;
	VkDevice m_device;


};
