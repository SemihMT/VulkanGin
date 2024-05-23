#pragma once
#include <memory>
#include <vector>


#include "vxl_Camera.h"
#include "vxl_Chunk.h"
#include "vxl_Device.h"

namespace vxl
{
	class vxlWorld final
	{
	public:
		const int Max_Chunks_Y{ 6 };
	public:
		vxlWorld(vxlDevice& device, vxlCamera* camera);
		~vxlWorld() = default;

		vxlChunk* GetChunkByWorldPos(const glm::vec3& worldPosition);
		vxlChunk* GetChunkByChunkPos(const glm::ivec3& chunkPosition);

		void Draw(VkCommandBuffer commandBuffer);
		void Update(float deltaTime);
	private:
		void Init();
		void GenerateChunk(const glm::ivec3& chunkPos);
		void GenerateChunk(const glm::vec3& worldPosition);
		void UnloadChunk(vxlChunk* chunk);


		vxlDevice& m_device;
		vxlCamera* m_camera;
		std::vector<std::unique_ptr<vxlChunk>> m_loadedChunks;

		

	};
}
