#pragma once
#include <vector>
#include "vxl_Device.h"
#include "vxl_Block.h"
namespace vxl
{
	class vxlChunk
	{
	public:
		struct Vertex3D
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec2 texCoord;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};
	public:
		static constexpr int ChunkSize = 16;
		vxlChunk(vxlDevice& device, const glm::ivec3& position);
		~vxlChunk();

		vxlChunk(const vxlChunk& other) = delete;
		vxlChunk(vxlChunk&& other) = delete;
		vxlChunk& operator=(const vxlChunk& other) = delete;
		vxlChunk& operator=(vxlChunk&& other) = delete;


		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
		void Destroy();

		glm::ivec3 GetChunkPos() const { return m_chunkPosition; }	// returns the position of the chunk based coordinate system. Each chunk is one element of the grid
		glm::ivec3 GetWorldPos() const { return m_worldPosition;  } // returns the world position of the chunk. Effectively all this does is multiply the chunk position with the chunk size
		vxlBlock& GetBlock(int x, int y, int z);
	private:
		std::array < glm::vec2,4 > GetTextureCoordinates(uint32_t textureIndex, int atlasSize = 32, int textureSize = 16, float offset = 1.25f);

		void GenerateChunk();
		void GenerateMesh();
		void AddFace(const glm::vec3& position, const glm::vec4& color, const glm::vec3& normal, uint32_t index);

		int GetIndex(int x, int y, int z) const;

		std::vector<vxlBlock> m_blocks;
		std::vector<Vertex3D> m_vertices;
		std::vector<uint32_t> m_indices;

		glm::ivec3 m_chunkPosition{};
		glm::ivec3 m_worldPosition{};

		void CreateVertexBuffers(const std::vector<Vertex3D>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);
		vxlDevice& m_device;
		//buffer
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;

		uint32_t m_vertexCount;
		uint32_t m_indexCount;
	};
	
}

