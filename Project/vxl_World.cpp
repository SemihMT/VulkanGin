#include "vxl_World.h"

#include <execution>

#include "vxl_Camera.h"
#include "vxl_Utils.h"

vxl::vxlWorld::vxlWorld(vxlDevice& device, vxlCamera* camera) :
	m_device{ device },
	m_camera{ camera }
{
	Init();
}

vxl::vxlChunk* vxl::vxlWorld::GetChunkByWorldPos(const glm::vec3& worldPosition)
{
	return GetChunkByChunkPos(GetChunkCoordinates(worldPosition));
}

vxl::vxlChunk* vxl::vxlWorld::GetChunkByChunkPos(const glm::ivec3& chunkPosition)
{
	auto it = std::find_if(
		std::execution::par_unseq, // Use parallel execution policy
		m_loadedChunks.begin(), m_loadedChunks.end(),
		[&chunkPosition](const std::unique_ptr<vxlChunk>& chunk) {
			return chunk->GetChunkPos() == chunkPosition;
		}
	);

	if (it != m_loadedChunks.end()) {
		return it->get();
	}
	else {
		return nullptr;
	}
}

void vxl::vxlWorld::Draw(VkCommandBuffer commandBuffer)
{
	for (const auto& chunk : m_loadedChunks)
	{
		chunk->Bind(commandBuffer);
		chunk->Draw(commandBuffer);
	}
}

void vxl::vxlWorld::Update(float deltaTime)
{
	glm::vec3 cameraPos{ m_camera->GetPosition() };
	glm::ivec3 currentChunkPos{ GetChunkCoordinates(cameraPos) };

	int radius = 3; // This will create a 3x3 area of chunks

	for (int x = currentChunkPos.x - radius; x <= currentChunkPos.x + radius; ++x)
	{
		for (int y = currentChunkPos.y - radius; y <= currentChunkPos.y + radius; ++y)
		{
			for (int z = currentChunkPos.z - radius; z <= currentChunkPos.z + radius; ++z)
			{
				glm::ivec3 chunkPos{ x, y, z };
				if (GetChunkByChunkPos(chunkPos) == nullptr && chunkPos.y < Max_Chunks_Y)
				{
					GenerateChunk(chunkPos);
				}
			}
		}
	}

	std::vector<std::unique_ptr<vxlChunk>> chunksToRemove;
	for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
		glm::ivec3 chunkPos = (*it)->GetChunkPos();
		int distanceX = abs(chunkPos.x - currentChunkPos.x);
		int distanceY = abs(chunkPos.y - currentChunkPos.y);
		int distanceZ = abs(chunkPos.z - currentChunkPos.z);

		if (distanceX > radius || distanceY > radius || distanceZ > radius) {
			chunksToRemove.push_back(std::move(*it));
			it = m_loadedChunks.erase(it);
		}
		else {
			++it;
		}
	}
	// Ensure the GPU has finished all work before destroying the buffers
	vkQueueWaitIdle(m_device.GetGraphicsQueue());


	chunksToRemove.clear();
}

void vxl::vxlWorld::Init()
{
	glm::vec3 cameraPos{ m_camera->GetPosition() };
	glm::ivec3 currentChunkPos{ GetChunkCoordinates(cameraPos) };

	int radius = 5;

	for (int x = currentChunkPos.x - radius; x <= currentChunkPos.x + radius; ++x)
	{
		for (int y = currentChunkPos.y - radius; y <= currentChunkPos.y; ++y)
		{
			for (int z = currentChunkPos.z - radius; z <= currentChunkPos.z + radius; ++z)
			{
				glm::ivec3 chunkPos{ x, y, z };
				GenerateChunk(chunkPos);
			}
		}
	}
}

void vxl::vxlWorld::GenerateChunk(const glm::ivec3& chunkPos)
{
	m_loadedChunks.push_back(std::make_unique<vxlChunk>(m_device, chunkPos));
}

void vxl::vxlWorld::GenerateChunk(const glm::vec3& worldPosition)
{
	GenerateChunk(GetChunkCoordinates(worldPosition));
}

void vxl::vxlWorld::UnloadChunk(vxlChunk* chunk)
{
	chunk->Destroy();
}
