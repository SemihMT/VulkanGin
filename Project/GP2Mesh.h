#pragma once
#include <vector>

#include "GP2CommandPool.h"
#include "GP2IndexBuffer.h"
#include "GP2VertexBuffer.h"

template <typename VertexType>
class GP2Mesh
{
public:
	GP2Mesh(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool,
	        const std::vector<VertexType>& vertices, const std::vector<uint16_t>& indices)
	{
		Initialize(device, physicalDevice, commandPool, vertices, indices);
	}

	~GP2Mesh() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool,
	                const std::vector<VertexType>& vertices, const std::vector<uint16_t>& indices)
	{
		m_commandPool = commandPool;
		m_vertices = vertices;
		m_indices = indices;

		m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(m_vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(m_indices);
	}

	void Destroy()
	{
		m_vertexBuffer.Destroy();
		m_indexBuffer.Destroy();
	}

	void Draw(const GP2CommandBuffer& cmdBuffer) const
	{
		vkCmdDrawIndexed(cmdBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
	}

	void AddTriangle(const Vertex2D& vertex)
	{
	}

	GP2VertexBuffer<VertexType> GetVertexBuffer() const { return m_vertexBuffer; }
	GP2IndexBuffer GetIndexBuffer() const { return m_indexBuffer; }

	void Bind(const GP2CommandBuffer& cmdBuffer) const
	{
		VkBuffer buffers[1]{ m_vertexBuffer.GetBuffer().GetVkBuffer() };
		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(cmdBuffer.GetVkCommandBuffer(), 0, 1, buffers, &offset);

		vkCmdBindIndexBuffer(cmdBuffer.GetVkCommandBuffer(), m_indexBuffer.GetBuffer().GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
	}

private:
	GP2CommandPool m_commandPool{};
	GP2CommandBuffer m_commandBuffer{};

	GP2VertexBuffer<VertexType> m_vertexBuffer{};
	GP2IndexBuffer m_indexBuffer{};

	std::vector<VertexType> m_vertices{};
	std::vector<uint16_t> m_indices{};
};

