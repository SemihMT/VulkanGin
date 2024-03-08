#pragma once
#include "vulkan/vulkan_core.h"

class GP2CommandBuffer
{
public:
	GP2CommandBuffer() = default;

	void SetVkCommandBuffer(VkCommandBuffer buffer) { m_commandBuffer = buffer; }
	VkCommandBuffer GetVkCommandBuffer() const { return m_commandBuffer; }

	void Reset() const;

	void BeginRecording() const;
	void EndRecording() const;
	void Submit(VkSubmitInfo& info) const;
private:
	VkCommandBuffer m_commandBuffer;
};
