#include "vxl_Chunk.h"

#include <iostream>
#include <glm/gtc/noise.hpp>

#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"

std::vector<VkVertexInputBindingDescription> vxl::vxlChunk::Vertex3D::GetBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex3D);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> vxl::vxlChunk::Vertex3D::GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex3D, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex3D, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

	return attributeDescriptions;
}

vxl::vxlChunk::vxlChunk(vxlDevice& device, const glm::ivec3& position) :
	m_blocks{ ChunkSize * ChunkSize * ChunkSize, vxlBlock{glm::vec3{0}, glm::vec4{1.0f}, vxlBlock::VoxelType::Solid,20} },
	m_chunkPosition{ position },
	m_worldPosition(position* ChunkSize),
	m_device{ device }
{
	GenerateChunk();
}

vxl::vxlChunk::~vxlChunk()
{
	//There is an upper limit to the number of allocations that can exist simultaneously!!!
	//This is only in the 1000s :(
	//We should eventually use something like the Vulkan Memory Allocator Library to help us allocate memory
	if(m_vertexBuffer != nullptr)
		Destroy();
}

void vxl::vxlChunk::Bind(VkCommandBuffer commandBuffer)
{
	if (!m_vertices.empty())
	{
		VkBuffer buffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

}

void vxl::vxlChunk::Draw(VkCommandBuffer commandBuffer)
{
	if (!m_vertices.empty())
	{
		vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);

	}
}

void vxl::vxlChunk::Destroy()
{
	if (!m_vertices.empty())
	{
		m_device.CleanupBuffer(m_vertexBuffer, m_vertexBufferMemory);
		m_device.CleanupBuffer(m_indexBuffer, m_indexBufferMemory);

	}
}


void vxl::vxlChunk::GenerateChunk()
{
	const float scale = 0.1f;

	for (int x = 0; x < ChunkSize; ++x)
	{
		for (int y = 0; y < ChunkSize; ++y)
		{
			for (int z = 0; z < ChunkSize; ++z)
			{
				glm::vec3 pos = glm::vec3(x + m_worldPosition.x, y + m_worldPosition.y, z + m_worldPosition.z);
				glm::vec3 scaledPos = pos * scale;
				float noiseValue = glm::perlin(scaledPos);

				noiseValue = (noiseValue + 1.0f) / 2.0f;

				vxlBlock::VoxelType blockType;
				if (noiseValue > 0.5f)
				{
					// Determine the block type based on the world Y position and Perlin noise
					if (pos.y < 80)
					{
						blockType = vxlBlock::VoxelType::Stone;
					}
					else if (pos.y < 95)
					{
						// Blend between Stone and Dirt using Perlin noise
						float transition = (pos.y - 80) / 15.0f;
						transition = noiseValue * transition;
						blockType = transition > 0.5f ? vxlBlock::VoxelType::Dirt : vxlBlock::VoxelType::Stone;
					}
					else
					{
						// Blend between Dirt and Grass using Perlin noise
						float transition = (pos.y - 95) / 5.0f;
						transition = noiseValue * transition;
						blockType = transition > 0.5f ? vxlBlock::VoxelType::Grass : vxlBlock::VoxelType::Dirt;
					}

					switch (blockType)
					{
					case vxlBlock::VoxelType::Air:
						break;
					case vxlBlock::VoxelType::Solid:
						break;
					case vxlBlock::VoxelType::Stone:
						m_blocks[GetIndex(x, y, z)].SetID({ 26, 6 });
						break;
					case vxlBlock::VoxelType::Dirt:
						m_blocks[GetIndex(x, y, z)].SetID({ 13, 21 });
						break;
					case vxlBlock::VoxelType::Grass:
						m_blocks[GetIndex(x, y, z)].SetID({ 8, 25 });
						break;
					case vxlBlock::VoxelType::Sand:
						m_blocks[GetIndex(x, y, z)].SetID({ 23, 16 });
						break;
					}

					m_blocks[GetIndex(x, y, z)].SetType(blockType);
					m_blocks[GetIndex(x, y, z)].SetPosition(pos);
				}
				else
				{
					m_blocks[GetIndex(x, y, z)].SetType(vxlBlock::VoxelType::Air);
					m_blocks[GetIndex(x, y, z)].SetPosition(pos);
				}
			}
		}
	}
	GenerateMesh();

}


void vxl::vxlChunk::GenerateMesh()
{
	m_vertices.clear();
	m_indices.clear();

	for (int x = 0; x < ChunkSize; ++x)
	{
		for (int y = 0; y < ChunkSize; ++y)
		{
			for (int z = 0; z < ChunkSize; ++z)
			{
				vxlBlock& voxel = GetBlock(x, y, z);
				if (voxel.GetType() != vxlBlock::VoxelType::Air)
				{
					// Add faces only if adjacent voxel is AIR
					if (x == 0 || GetBlock(x - 1, y, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(-1, 0, 0), voxel.GetID());
					}
					if (x == ChunkSize - 1 || GetBlock(x + 1, y, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(1, 0, 0), voxel.GetID());
					}
					if (y == 0 || GetBlock(x, y - 1, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(0, -1, 0), voxel.GetID());
					}
					if (y == ChunkSize - 1 || GetBlock(x, y + 1, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(0, 1, 0), voxel.GetID());
					}
					if (z == 0 || GetBlock(x, y, z - 1).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(0, 0, -1), voxel.GetID());
					}
					if (z == ChunkSize - 1 || GetBlock(x, y, z + 1).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), voxel.GetColor(), glm::vec3(0, 0, 1), voxel.GetID());
					}
				}
			}
		}
	}

	CreateVertexBuffers(m_vertices);
	CreateIndexBuffers(m_indices);
}


std::array<glm::vec2, 4> vxl::vxlChunk::GetTextureCoordinates(uint32_t textureIndex, int atlasSize, int textureSize, float offset)
{

	// Calculate the row and column index of the texture in the atlas
	int xIndex = textureIndex % atlasSize;
	int yIndex = textureIndex / atlasSize;

	// Size of one texel in normalized coordinates
	float texelSize = 1.0f / (atlasSize * textureSize);
	float minU = xIndex * textureSize * texelSize;
	float minV = yIndex * textureSize * texelSize;
	float maxU = (xIndex + 1) * textureSize * texelSize;
	float maxV = (yIndex + 1) * textureSize * texelSize;

	// Apply offset to prevent texture bleeding
	minU += offset * texelSize;
	minV += offset * texelSize;
	maxU -= offset * texelSize;
	maxV -= offset * texelSize;

	return {

		glm::vec2(maxU, minV), // Top-right
		glm::vec2(minU, minV), // Top-left
		glm::vec2(minU, maxV), // Bottom-left
		glm::vec2(maxU, maxV) // Bottom-right
	};
}

void vxl::vxlChunk::AddFace(const glm::vec3& position, const glm::vec4& color, const glm::vec3& normal, uint32_t index)
{

	glm::vec3 faceVertices[4];
	std::array<glm::vec2, 4> texCoords;

	// Define the vertices of the face and corresponding texture coordinates based on the normal direction
	if (normal == glm::vec3(-1, 0, 0)) { // -X face
		faceVertices[0] = position + glm::vec3(0, 1, 0);
		faceVertices[1] = position + glm::vec3(0, 1, 1);
		faceVertices[2] = position + glm::vec3(0, 0, 1);
		faceVertices[3] = position + glm::vec3(0, 0, 0);
		texCoords = GetTextureCoordinates(index, 32);
	}
	else if (normal == glm::vec3(1, 0, 0)) { // +X face
		faceVertices[2] = position + glm::vec3(1, 0, 0);
		faceVertices[3] = position + glm::vec3(1, 0, 1);
		faceVertices[0] = position + glm::vec3(1, 1, 1);
		faceVertices[1] = position + glm::vec3(1, 1, 0);
		texCoords = GetTextureCoordinates(index, 32);
	}
	else if (normal == glm::vec3(0, -1, 0)) { // -Y face
		faceVertices[1] = position + glm::vec3(0, 0, 0);
		faceVertices[2] = position + glm::vec3(0, 0, 1);
		faceVertices[3] = position + glm::vec3(1, 0, 1);
		faceVertices[0] = position + glm::vec3(1, 0, 0);
		texCoords = GetTextureCoordinates(index, 32);
	}
	else if (normal == glm::vec3(0, 1, 0)) { // +Y face
		faceVertices[0] = position + glm::vec3(0, 1, 0);
		faceVertices[1] = position + glm::vec3(1, 1, 0);
		faceVertices[2] = position + glm::vec3(1, 1, 1);
		faceVertices[3] = position + glm::vec3(0, 1, 1);
		texCoords = GetTextureCoordinates(index, 32);
	}
	else if (normal == glm::vec3(0, 0, -1)) { // -Z face
		faceVertices[2] = position + glm::vec3(0, 0, 0);
		faceVertices[3] = position + glm::vec3(1, 0, 0);
		faceVertices[0] = position + glm::vec3(1, 1, 0);
		faceVertices[1] = position + glm::vec3(0, 1, 0);
		texCoords = GetTextureCoordinates(index, 32);
	}
	else if (normal == glm::vec3(0, 0, 1)) { // +Z face
		faceVertices[3] = position + glm::vec3(0, 0, 1);
		faceVertices[0] = position + glm::vec3(0, 1, 1);
		faceVertices[1] = position + glm::vec3(1, 1, 1);
		faceVertices[2] = position + glm::vec3(1, 0, 1);
		texCoords = GetTextureCoordinates(index, 32);
	}


	// Add vertices with texture coordinates and color
	for (int i = 0; i < 4; ++i) {
		m_vertices.push_back({ faceVertices[i], color, texCoords[i] });
	}

	uint32_t idx = m_vertices.size();
	m_indices.push_back(idx - 4);
	m_indices.push_back(idx - 3);
	m_indices.push_back(idx - 2);
	m_indices.push_back(idx - 2);
	m_indices.push_back(idx - 1);
	m_indices.push_back(idx - 4);
}

void vxl::vxlChunk::CreateVertexBuffers(const std::vector<Vertex3D>& vertices)
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

void vxl::vxlChunk::CreateIndexBuffers(const std::vector<uint32_t>& indices)
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

vxl::vxlBlock& vxl::vxlChunk::GetBlock(int x, int y, int z)
{
	return m_blocks[GetIndex(x, y, z)];
}

int vxl::vxlChunk::GetIndex(int x, int y, int z) const
{
	return x + ChunkSize * (y + ChunkSize * z);
}
