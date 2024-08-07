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
		virtual void OnKeyPress(int key, int scancode, int action, int mods) = 0;
	};

	class vxlWorld final : public WorldObserver 
	{
	public:
		const int Max_Chunks_Y{ 6 };

	public:
		vxlWorld(vxlDevice& device, vxlCamera* camera);
		~vxlWorld() = default;

		vxlWorld(const vxlWorld& other) = delete;
		vxlWorld(vxlWorld&& other) = delete;
		vxlWorld& operator=(const vxlWorld& other) = delete;
		vxlWorld& operator=(vxlWorld&& other) = delete;

		vxlChunk* GetChunkByWorldPos(const glm::vec3& worldPosition);			// Returns a pointer to a chunk at the requested world coordinates, nullptr if there is no chunk there
		vxlChunk* GetChunkByChunkPos(const glm::ivec3& chunkPosition);			// Returns a pointer to a chunk at the requested chunk coordinates, nullptr if there is no chunk there

		void OnMouseButton(int button, int action, int mods) override;			// Performs a raycast and related logic when there is a mouse button press event
		void OnKeyPress(int key, int scancode, int action, int mods) override;	// Changes the currently selected block 

		void Draw(VkCommandBuffer commandBuffer);
		void Update(float deltaTime);
		void SetCurrentSelectedBlock(vxlBlock::VoxelType block) { m_currentSelectedBlock = block; }
	private:
		void Init();			// Generates the chunks around the player
		int m_initRadius{4};	// Will cause 'Init' to generate 9x9x9 chunks around the player
		int m_updateRadius {3}; // Will cause 'Update' to keep 7x7x7 chunks loaded around the player
		int m_removeRadius {8};	// Will cause any chunks beyond a 17x17x17 cube around the player to be removed from memory
		int m_viewRadius{4};	// Will only render all chunks within a 9x9x9 cube around the player

		
		// Chunk generation
		void GenerateChunk(const glm::ivec3& chunkPos);
		void GenerateChunk(const glm::vec3& worldPosition);
		void GenerateChunksWithinRadius(const glm::ivec3& chunkPos, int radius);
		void RemoveChunksBeyondRadius(const glm::ivec3& chunkPos, int radius);

		// Raycasting:
		// Uses the 3D Digital Differential Analyzer algorithm:
		// This algorithm steps incrementally through each voxel from the origin of the ray until it reaches the endpoint
		// This incremental stepping by one voxel size each time makes it incredibly fast while allowing us to query what the voxel at the current step is
		RaycastHit PerformRaycast(const Ray& ray, float maxDistance = 10.f);
		vxlBlock* GetBlock(const glm::vec3& worldPosition);


		vxlDevice& m_device;
		vxlCamera* m_camera;
		std::vector<std::unique_ptr<vxlChunk>> m_loadedChunks;
		vxlBlock::VoxelType m_currentSelectedBlock{ vxlBlock::VoxelType::OakPlanks };
		int m_seed{};

	};
}
