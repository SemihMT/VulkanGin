#pragma once
#include <memory>
#include <vector>


#include "vxl_Camera.h"
#include "vxl_Chunk.h"
#include "vxl_Device.h"
#include "vxl_Utils.h"

namespace vxl
{

	class WorldObserver {
	public:
		virtual void OnMouseButton(int button, int action, int mods) = 0;
	};

	class vxlWorld final : public WorldObserver
	{
	public:
		const int Max_Chunks_Y{ 6 };
	public:
		vxlWorld(vxlDevice& device, vxlCamera* camera);
		~vxlWorld() = default;

		vxlChunk* GetChunkByWorldPos(const glm::vec3& worldPosition);
		vxlChunk* GetChunkByChunkPos(const glm::ivec3& chunkPosition);
		void OnMouseButton(int button, int action, int mods);

		void Draw(VkCommandBuffer commandBuffer);
		void Update(float deltaTime);
	private:
		void Init();
		void GenerateChunk(const glm::ivec3& chunkPos);
		void GenerateChunk(const glm::vec3& worldPosition);
		void UnloadChunk(vxlChunk* chunk);


		RaycastHit PerformRaycast(const Ray& ray, float maxDistance = 10.f);
		


		vxlDevice& m_device;
		vxlCamera* m_camera;
		std::vector<std::unique_ptr<vxlChunk>> m_loadedChunks;
		
		

	};
}
