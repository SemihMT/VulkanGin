#pragma once
#include "vulkanbase/VulkanBase.h"
class GP2CommandManager final
{
public:
	GP2CommandManager(VkPhysicalDevice physicalDevice, VkDevice device);

	//RO5
	~GP2CommandManager();
	GP2CommandManager(const GP2CommandManager& other) = delete;
	GP2CommandManager(GP2CommandManager&& other) = delete;
	GP2CommandManager& operator=(const GP2CommandManager& other) = delete;
	GP2CommandManager& operator=(GP2CommandManager&& other) = delete;
	



	void drawFrame(uint32_t imageIndex);
	void createCommandBuffer();
	void createCommandPool();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;

};
