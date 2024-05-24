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
	int viewDistance = 4;
	for (const auto& chunk : m_loadedChunks)
	{
		glm::vec3 cameraPos{ m_camera->GetPosition() };
		glm::ivec3 currentChunkPos{ GetChunkCoordinates(cameraPos) };
		glm::ivec3 chunkPos = chunk->GetChunkPos();
		const int squaredDistance =
			(chunkPos.x - currentChunkPos.x) * (chunkPos.x - currentChunkPos.x) +
			(chunkPos.y - currentChunkPos.y) * (chunkPos.y - currentChunkPos.y) +
			(chunkPos.z - currentChunkPos.z) * (chunkPos.z - currentChunkPos.z);

		if (squaredDistance <= viewDistance * viewDistance)
		{

			chunk->Bind(commandBuffer);
			chunk->Draw(commandBuffer);

		}
	}
}
void vxl::vxlWorld::Update(float deltaTime)
{
	// Get camera position
	glm::vec3 cameraPos = m_camera->GetPosition();

	// Define chunk radii
	constexpr int loadRadius = 3; // Load chunks within a 3x3x3 cube
	constexpr int deleteRadiusSquared = 5 * 5; // Squared delete radius

	// Calculate current chunk position
	glm::ivec3 currentChunkPos = GetChunkCoordinates(cameraPos);

	// Load new chunks within load radius
	for (int x = currentChunkPos.x - loadRadius; x <= currentChunkPos.x + loadRadius; ++x)
	{
		for (int y = currentChunkPos.y - loadRadius; y <= currentChunkPos.y + loadRadius; ++y)
		{
			for (int z = currentChunkPos.z - loadRadius; z <= currentChunkPos.z + loadRadius; ++z)
			{
				glm::ivec3 chunkPos{ x, y, z };
				int dx = chunkPos.x - currentChunkPos.x;
				int dy = chunkPos.y - currentChunkPos.y;
				int dz = chunkPos.z - currentChunkPos.z;
				int distanceSquared = dx * dx + dy * dy + dz * dz;

				if (GetChunkByChunkPos(chunkPos) == nullptr && distanceSquared <= loadRadius * loadRadius)
				{
					GenerateChunk(chunkPos);
				}
			}
		}
	}

	// Remove chunks beyond delete radius
	auto it = m_loadedChunks.begin();
	while (it != m_loadedChunks.end())
	{
		glm::ivec3 chunkPos = (*it)->GetChunkPos();
		int dx = chunkPos.x - currentChunkPos.x;
		int dy = chunkPos.y - currentChunkPos.y;
		int dz = chunkPos.z - currentChunkPos.z;
		int distanceSquared = dx * dx + dy * dy + dz * dz;

		if (distanceSquared > deleteRadiusSquared)
		{
			it = m_loadedChunks.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Ensure GPU has finished work
	vkQueueWaitIdle(m_device.GetGraphicsQueue());
}

void vxl::vxlWorld::Init()
{
	// Get camera position
	glm::vec3 cameraPos = m_camera->GetPosition();

	// Calculate current chunk position
	glm::ivec3 currentChunkPos = GetChunkCoordinates(cameraPos);

	// Define chunk radii
	constexpr int radius = 4;

	// Iterate over chunks within the specified radius
	for (int x = currentChunkPos.x - radius; x <= currentChunkPos.x + radius; ++x)
	{
		for (int y = currentChunkPos.y - radius; y <= currentChunkPos.y + radius; ++y)
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
