#pragma once
#include <vector>

#include "GP2CommandPool.h"
#include "GP2IndexBuffer.h"
#include "GP2VertexBuffer.h"
struct Vertex;

class GP2Mesh
{
public:
	GP2Mesh(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	~GP2Mesh() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
	void Destroy();
	void Draw(const GP2CommandBuffer& cmdBuffer) const;
	void AddTriangle(const Vertex& vertex); 
	GP2VertexBuffer GetVertexBuffer() const { return m_vertexBuffer; }
	GP2IndexBuffer GetIndexBuffer() const { return m_indexBuffer; }
	void Bind(const GP2CommandBuffer& cmdBuffer) const;

private:
	GP2CommandPool m_commandPool{};
	GP2CommandBuffer m_commandBuffer{};

	GP2VertexBuffer m_vertexBuffer{};
	GP2IndexBuffer m_indexBuffer{};

	std::vector<Vertex> m_vertices{};
	std::vector<uint16_t> m_indices{};
};
