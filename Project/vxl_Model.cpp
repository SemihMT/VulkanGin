#include "vxl_Model.h"

namespace vxl
{
	std::vector<VkVertexInputBindingDescription> vxlModel::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> vxlModel::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	vxlModel::vxlModel(vxlDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) :
		m_device{ device }
	{
		CreateVertexBuffers(vertices);
		CreateIndexBuffers(indices);
	}

	vxlModel::~vxlModel()
	{
		//There is an upper limit to the number of allocations that can exist simultaneously!!!
		//This is only in the 1000s :(
		//We should eventually use something like the Vulkan Memory Allocator Library to help us allocate memory
		m_device.CleanupBuffer(m_vertexBuffer, m_vertexBufferMemory);
		m_device.CleanupBuffer(m_indexBuffer, m_indexBufferMemory);
	}

	void vxlModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "VertexCount must be at least 3!");

		const VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;


		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		m_device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		//Creates a region of memory accessible to the CPU that is also mapped to a region of memory on the GPU
		//Sets 'data' to the beginning of the mapped CPU region range
		vkMapMemory(m_device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		//Copies the vertices we want to send to the GPU to the CPU region we mapped
		//Because we set up the buffer 'm_vertexBuffer' to use 'HOST_COHERENT_BIT', the CPU memory automatically gets propagated to device memory
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		//We've sent the vertex data to the gpu, no need to keep the cpu data mapped any longer
		vkUnmapMemory(m_device.GetDevice(), stagingBufferMemory);

		//Host: CPU
		//Device: GPU
		m_device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_vertexBuffer, m_vertexBufferMemory);

		m_device.CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		m_device.CleanupBuffer(stagingBuffer, stagingBufferMemory);



	}

	void vxlModel::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_indexCount = static_cast<uint32_t>(indices.size());
		const VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;


		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		m_device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(m_device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_device.GetDevice(), stagingBufferMemory);

		m_device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_indexBuffer,
			m_indexBufferMemory);

		m_device.CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

		m_device.CleanupBuffer(stagingBuffer, stagingBufferMemory);
	}

	void vxlModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void vxlModel::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
	}


}