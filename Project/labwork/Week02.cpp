#include "vulkanbase/VulkanBase.h"


void VulkanBase::recordCommandBuffer(GP2CommandBuffer commandBuffer, uint32_t imageIndex) {
	
	drawFrame(imageIndex);

}

void VulkanBase::drawFrame(uint32_t imageIndex) {

	//This function should get factorized into each separate
	//element's setup or something

	//binds the renderpass to the framebuffer we can render to
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	auto commandBuffer = m_commandBuffer.GetVkCommandBuffer();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	//This should get the vertexbuffers from all meshes
	//std::vector<VkBuffer> vertexBuffers{};
	//VkBuffer vertexBuffers[] = { m_vertexBuffer.GetBuffer().GetVkBuffer() };
	//VkDeviceSize offsets[] = { 0 };
	//vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), offsets);
	//vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetBuffer().GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
	
	
	

	//Describes to what portion of the framebuffer we render
	//Will almost always be 0,0 to window width, height
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	//The scissor filter that discards any pixels that is not inside the extent it defines
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	drawScene();
	vkCmdEndRenderPass(commandBuffer);
}

QueueFamilyIndices VulkanBase::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}