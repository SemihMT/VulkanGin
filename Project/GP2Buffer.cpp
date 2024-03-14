#include "GP2Buffer.h"

#include "GP2CommandPool.h"

void GP2Buffer::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool)
{
	m_device = device;
	m_physicalDevice = physicalDevice;
    m_commandPool = commandPool;
}

void GP2Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_device, m_buffer, m_bufferMemory, 0);
}

void GP2Buffer::Destroy()
{
    vkDestroyBuffer(m_device, m_buffer, nullptr);
    vkFreeMemory(m_device, m_bufferMemory, nullptr);
}

void GP2Buffer::CopyBuffer(GP2Buffer dstBuffer, VkDeviceSize size)
{
    auto cmdBuffer = m_commandPool.createCommandBuffer();
    auto vkCmdBuffer = cmdBuffer.GetVkCommandBuffer();
    cmdBuffer.BeginRecording();
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(vkCmdBuffer,GetVkBuffer(), dstBuffer.GetVkBuffer(), 1, &copyRegion);
    cmdBuffer.EndRecording();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    cmdBuffer.Submit(submitInfo);

    //HMM this needs a VkQueue to work...
    //Should i place a reference to the available queues in the commandpool?

    //Getting the handle to the graphics queue -> THIS ALREADY EXISTS IN VULKANBASE, WE'RE ESSENTIALLY DOING DOUBLE THE WORK
    // I DONT WANT TO CREATE A SINGLETON FOR THESE OBJECTS AND I AM TOO LAZY TO USE THE SERVICE LOCATOR PATTERN HERE
    // TODO: Don't get the graphics queue again after it already is stored in vulkanBase, use a singleton/service locator/dependency injection to use the available resources
    VkQueue graphicsQueue;
    vkGetDeviceQueue(m_device, m_commandPool.GetQueueFamiliyIndices().graphicsFamily.value(), 0, &graphicsQueue);

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(m_device, m_commandPool.GetVkCommandPool(), 1, &vkCmdBuffer);
}


uint32_t GP2Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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
