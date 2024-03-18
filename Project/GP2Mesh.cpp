#include "GP2Mesh.h"

GP2Mesh::GP2Mesh(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
	Initialize(device, physicalDevice, commandPool, vertices, indices);
}

void GP2Mesh::Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
{
	m_commandPool = commandPool;
	m_vertices = vertices;
	m_indices = indices;

	m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
	m_vertexBuffer.CreateVertexBuffer(m_vertices);

	m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
	m_indexBuffer.CreateIndexBuffer(m_indices);
}

void GP2Mesh::Destroy()
{
	m_vertexBuffer.Destroy();
	m_indexBuffer.Destroy();
}

void GP2Mesh::Draw(const GP2CommandBuffer& cmdBuffer) const
{
	vkCmdDrawIndexed(cmdBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
}

void GP2Mesh::AddTriangle(const Vertex& vertex)
{
}

void GP2Mesh::Bind(const GP2CommandBuffer& cmdBuffer) const
{
	VkBuffer buffers[1] { m_vertexBuffer.GetBuffer().GetVkBuffer() };
	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(cmdBuffer.GetVkCommandBuffer(), 0,1, buffers, &offset);

	vkCmdBindIndexBuffer(cmdBuffer.GetVkCommandBuffer(), m_indexBuffer.GetBuffer().GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
}
