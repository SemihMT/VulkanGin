#include "vxl_Chunk.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <glm/gtc/noise.hpp>

#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "GP2Buffer.h"
#include "vxl_Utils.h"

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
	m_blocks{ ChunkSize * ChunkSize * ChunkSize, vxlBlock{glm::vec3{0}, vxlBlock::VoxelType::TEST_BLOCK} },
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
	if (m_vertexBuffer != nullptr)
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

void vxl::vxlChunk::RemoveBlock(const glm::ivec3& blockPos)
{
	m_device.CleanupBuffer(m_vertexBuffer, m_vertexBufferMemory);
	m_device.CleanupBuffer(m_indexBuffer, m_indexBufferMemory);
	auto index = GetIndex(blockPos.x, blockPos.y, blockPos.z);
	m_blocks[index].SetType(vxlBlock::VoxelType::Air);

	
	GenerateMesh();
}
void vxl::vxlChunk::PlaceBlock(const glm::ivec3& blockPos, vxlBlock::VoxelType type)
{
	m_device.CleanupBuffer(m_vertexBuffer, m_vertexBufferMemory);
	m_device.CleanupBuffer(m_indexBuffer, m_indexBufferMemory);
	m_blocks[GetIndex(blockPos.x, blockPos.y, blockPos.z)].SetType(type);
	
	GenerateMesh();
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
	// Constants
	constexpr float surfaceScale = 0.09f;
	constexpr float caveScale = 0.1f;
	constexpr int caveThreshold = 40;
	constexpr int maxHeight = 128;
	constexpr int octaves = 5;
	constexpr float persistence = 0.5f;
	constexpr float lacunarity = 2.0f;

	// Precompute the world position offset
	const glm::vec3 worldOffset(m_worldPosition.x, m_worldPosition.y, m_worldPosition.z);

	// Prepare a vector to hold block positions and types
	struct BlockInfo {
		glm::ivec3 position;
		vxlBlock::VoxelType type;
	};
	std::vector<BlockInfo> blocks(ChunkSize * ChunkSize * ChunkSize);

	// Fill the block positions and types
	std::for_each(std::execution::par_unseq, blocks.begin(), blocks.end(), [&](BlockInfo& block) {
		// Compute block coordinates
		const int index = &block - blocks.data();
		const int x = index / (ChunkSize * ChunkSize);
		const int y = (index / ChunkSize) % ChunkSize;
		const int z = index % ChunkSize;

		const glm::vec3 pos = glm::vec3(x, y, z) + worldOffset;

		// Adjust the surface scale dynamically based on the chunk's vertical position
		const float adjustedSurfaceScale = surfaceScale * (1.0f - (pos.y / static_cast<float>(maxHeight)));
		const float surfaceNoise = GenerateLayeredNoise(pos, octaves, persistence, lacunarity, adjustedSurfaceScale);

		float caveNoise = glm::simplex(pos * caveScale);
		caveNoise = (caveNoise + 1.0f) / 2.0f;

		vxlBlock::VoxelType blockType = vxlBlock::VoxelType::Air;
		const int height = static_cast<int>(surfaceNoise * ChunkSize); // Convert noise to height

		// Introduce a gradient to reduce block density at higher elevations
		const float gradient = glm::clamp((maxHeight - pos.y) / static_cast<float>(maxHeight), 0.0f, 1.0f);
		if (surfaceNoise * gradient > 0.1f) { // Adjust this threshold for desired smoothness
			if (pos.y > caveThreshold && pos.y < 102) {
				// Mountainous terrain
				blockType = GenerateMountainTerrain(pos, surfaceNoise);
			}
			else if (pos.y <= caveThreshold) {
				// Cave-like terrain
				blockType = GenerateCaveTerrain(pos, caveNoise, surfaceNoise, caveThreshold);
			}
			else if (pos.y > caveThreshold && pos.y < maxHeight && y <= height) {
				blockType = vxlBlock::VoxelType::GrassSide;
			}
		}

		block.position = glm::ivec3(x, y, z);
		block.type = blockType;
		});

	// Update blocks in the world
	std::for_each(std::execution::par_unseq, blocks.begin(), blocks.end(), [&](const BlockInfo& block) {
		const int index = GetIndex(block.position.x, block.position.y, block.position.z);
		m_blocks[index].SetType(block.type);
		m_blocks[index].SetPosition(glm::vec3(block.position) + worldOffset);
		});

	// Generate a mesh only if the chunk has blocks in it
	if (!std::ranges::all_of(m_blocks, [&](const vxlBlock& block) { return block.GetType() == vxlBlock::VoxelType::Air; })) {
		GenerateMesh();
	}
}

vxl::vxlBlock::VoxelType vxl::vxlChunk::GenerateMountainTerrain(const glm::vec3& pos, float surfaceNoise)
{

	if (surfaceNoise > 0.5f)
	{
		if (pos.y < 80)
		{
			return vxlBlock::VoxelType::Stone;
		}
		else if (pos.y < 95)
		{
			float transition = (pos.y - 80) / 15.0f;
			transition = surfaceNoise * transition;
			return transition > 0.5f ? vxlBlock::VoxelType::Dirt : vxlBlock::VoxelType::Stone;
		}
		else if (pos.y < 102)
		{
			float transition = (pos.y - 95) / 5.0f;
			transition = surfaceNoise * transition;
			return transition > 0.5f ? vxlBlock::VoxelType::GrassSide: vxlBlock::VoxelType::Dirt;
		}
	}

	return vxlBlock::VoxelType::Air;
}

vxl::vxlBlock::VoxelType vxl::vxlChunk::GenerateCaveTerrain(const glm::vec3& pos, float caveNoise, float surfaceNoise, int caveThreshold)
{
	// Blend between cave and surface noise based on height
	float blendFactor = static_cast<float>(caveThreshold - pos.y) / caveThreshold;
	float noiseValue = glm::mix(caveNoise, surfaceNoise, blendFactor);

	if (noiseValue > 0.5f)
	{
		return vxlBlock::VoxelType::Air;
	}
	else
	{
		return vxlBlock::VoxelType::Stone;
	}
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
						AddFace(voxel.GetPosition(), glm::vec3(-1, 0, 0), voxel.GetType());
					}
					if (x == ChunkSize - 1 || GetBlock(x + 1, y, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), glm::vec3(1, 0, 0), voxel.GetType());
					}
					if (y == 0 || GetBlock(x, y - 1, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), glm::vec3(0, -1, 0), voxel.GetType());
					}
					if (y == ChunkSize - 1 || GetBlock(x, y + 1, z).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), glm::vec3(0, 1, 0), voxel.GetType());
					}
					if (z == 0 || GetBlock(x, y, z - 1).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), glm::vec3(0, 0, -1), voxel.GetType());
					}
					if (z == ChunkSize - 1 || GetBlock(x, y, z + 1).GetType() == vxlBlock::VoxelType::Air) {
						AddFace(voxel.GetPosition(), glm::vec3(0, 0, 1), voxel.GetType());
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

void vxl::vxlChunk::AddFace(const glm::vec3& position, const glm::vec3& normal, vxlBlock::VoxelType type)
{

	glm::vec3 faceVertices[4]{};
	std::array<glm::vec2, 4> texCoords;
	glm::vec3 shadowMultiplier{};

	// Define the vertices of the face and corresponding texture coordinates based on the normal direction
	if (normal == glm::vec3(-1, 0, 0)) { // -X face
		faceVertices[0] = position + glm::vec3(0, 1, 0);
		faceVertices[1] = position + glm::vec3(0, 1, 1);
		faceVertices[2] = position + glm::vec3(0, 0, 1);
		faceVertices[3] = position + glm::vec3(0, 0, 0);
		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);
	}
	else if (normal == glm::vec3(1, 0, 0)) { // +X face
		faceVertices[0] = position + glm::vec3(1, 1, 1);
		faceVertices[1] = position + glm::vec3(1, 1, 0);
		faceVertices[2] = position + glm::vec3(1, 0, 0);
		faceVertices[3] = position + glm::vec3(1, 0, 1);

		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);

	}
	else if (normal == glm::vec3(0, -1, 0)) { // -Y face
		faceVertices[0] = position + glm::vec3(1, 0, 0);
		faceVertices[1] = position + glm::vec3(0, 0, 0);
		faceVertices[2] = position + glm::vec3(0, 0, 1);
		faceVertices[3] = position + glm::vec3(1, 0, 1);
		if (type == vxlBlock::VoxelType::GrassSide)
		{
			type = vxlBlock::VoxelType::Dirt;
		}
		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);

	}
	else if (normal == glm::vec3(0, 1, 0)) { // +Y face
		faceVertices[0] = position + glm::vec3(0, 1, 0);
		faceVertices[1] = position + glm::vec3(1, 1, 0);
		faceVertices[2] = position + glm::vec3(1, 1, 1);
		faceVertices[3] = position + glm::vec3(0, 1, 1);
		if(type == vxlBlock::VoxelType::GrassSide)
		{
			type = vxlBlock::VoxelType::GrassTop;
		}
		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);

	}
	else if (normal == glm::vec3(0, 0, -1)) { // -Z face
		faceVertices[0] = position + glm::vec3(1, 1, 0);
		faceVertices[1] = position + glm::vec3(0, 1, 0);
		faceVertices[2] = position + glm::vec3(0, 0, 0);
		faceVertices[3] = position + glm::vec3(1, 0, 0);

		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);

	}
	else if (normal == glm::vec3(0, 0, 1)) { // +Z face
		faceVertices[0] = position + glm::vec3(0, 1, 1);
		faceVertices[1] = position + glm::vec3(1, 1, 1);
		faceVertices[2] = position + glm::vec3(1, 0, 1);
		faceVertices[3] = position + glm::vec3(0, 0, 1);
		texCoords = GetTextureCoordinates(static_cast<uint32_t>(type), 32);
		shadowMultiplier = CalculateShadowMultiplier(position, normal);

	}


	// Add vertices with texture coordinates and color
	for (int i = 0; i < 4; ++i) {
		m_vertices.push_back({ faceVertices[i], shadowMultiplier, texCoords[i] });
	}

	uint32_t idx = static_cast<uint32_t>(m_vertices.size());
	m_indices.push_back(idx - 4);
	m_indices.push_back(idx - 3);
	m_indices.push_back(idx - 2);
	m_indices.push_back(idx - 2);
	m_indices.push_back(idx - 1);
	m_indices.push_back(idx - 4);
}

glm::vec3 vxl::vxlChunk::CalculateShadowMultiplier(const glm::vec3& pos, const glm::vec3& normal,
	const glm::vec3& lightPos)
{
	// Calculate direction from the face to the light source
	glm::vec3 lightDirection = glm::normalize(lightPos - pos);

	// Calculate the dot product of face normal and light direction
	float dotProduct = glm::dot(normal, lightDirection);

	// Clamp dot product to [0, 1] range to avoid negative shadow values
	dotProduct = glm::clamp(dotProduct, 0.0f, 1.0f);

	// Use the dot product as the shadow multiplier
	return glm::vec3(dotProduct);
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
