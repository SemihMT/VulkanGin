#include "GP2CommandBuffer.h"

#include <stdexcept>

void GP2CommandBuffer::Reset() const
{
	vkResetCommandBuffer(m_commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void GP2CommandBuffer::BeginRecording() const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; //optional
	beginInfo.pInheritanceInfo = nullptr; //optional

	if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

}

void GP2CommandBuffer::EndRecording() const
{
	if(vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to end recording the command buffer!");
	}
}

void GP2CommandBuffer::Submit(VkSubmitInfo& info) const
{
	info.commandBufferCount = 1;
	info.pCommandBuffers = &m_commandBuffer;
}
