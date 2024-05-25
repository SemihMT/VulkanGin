#include "vxl_World.h"

#include <execution>

#include "vxl_Camera.h"
#include "vxl_Utils.h"

vxl::vxlWorld::vxlWorld(vxlDevice& device, vxlCamera* camera) :
	m_device{ device },
	m_camera{ camera }
{
	m_camera->AddObserver(this);
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

void vxl::vxlWorld::OnMouseButton(int button, int action, int mods)
{
	vkQueueWaitIdle(m_device.GetGraphicsQueue());
	Ray r{ m_camera->GetPosition(), glm::normalize(m_camera->GetForward()) };
	auto hit = PerformRaycast(r);
	if (hit.hit)
	{
		auto hitChunk = GetChunkByWorldPos(hit.hitPosition);
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			hitChunk->RemoveBlock(hit.voxelCoords);

		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			glm::ivec3 chunkOffset(0);
			glm::ivec3 localPlacementCoords = hit.voxelCoords + glm::ivec3{ hit.hitNormal };

			// Adjust chunk and local coordinates if necessary
			if (localPlacementCoords.x < 0) {
				chunkOffset.x -= 1;
				localPlacementCoords.x = vxlChunk::ChunkSize - 1;
			}
			else if (localPlacementCoords.x >= vxlChunk::ChunkSize) {
				chunkOffset.x += 1;
				localPlacementCoords.x = 0;
			}

			if (localPlacementCoords.y < 0) {
				chunkOffset.y -= 1;
				localPlacementCoords.y = vxlChunk::ChunkSize - 1;
			}
			else if (localPlacementCoords.y >= vxlChunk::ChunkSize) {
				chunkOffset.y += 1;
				localPlacementCoords.y = 0;
			}

			if (localPlacementCoords.z < 0) {
				chunkOffset.z -= 1;
				localPlacementCoords.z = vxlChunk::ChunkSize - 1;
			}
			else if (localPlacementCoords.z >= vxlChunk::ChunkSize) {
				chunkOffset.z += 1;
				localPlacementCoords.z = 0;
			}

			// Get the chunk where the block will be placed
			auto placedBlockChunk = GetChunkByChunkPos(hitChunk->GetChunkPos() + chunkOffset);

			// Place the new block in the calculated position
			if (placedBlockChunk) {
				if (placedBlockChunk->GetBlock(localPlacementCoords.x, localPlacementCoords.y, localPlacementCoords.z ).GetType() == vxlBlock::VoxelType::Air) {
					// Place the new block in the calculated position if it's not occupied
					placedBlockChunk->PlaceBlock(localPlacementCoords, vxlBlock::VoxelType::OakPlanks);
				}
			}
		}

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
	constexpr int deleteRadiusSquared = 8 * 8; // Squared delete radius

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

vxl::RaycastHit vxl::vxlWorld::PerformRaycast(const Ray& ray, float maxDistance)
{
	const int ChunkSize = 16;
	const float VoxelSize = 1.0f; // Assuming each voxel is 1 unit in size
	RaycastHit hit{};

	glm::vec3 currentRayPos = ray.origin;
	glm::vec3 rayDir = glm::normalize(ray.direction);

	// Calculate initial voxel coordinates
	int voxelX = static_cast<int>(std::floor(currentRayPos.x));
	int voxelY = static_cast<int>(std::floor(currentRayPos.y));
	int voxelZ = static_cast<int>(std::floor(currentRayPos.z));

	// Calculate the direction to step in each axis (1 or -1)
	glm::ivec3 step(
		rayDir.x > 0 ? 1 : -1,
		rayDir.y > 0 ? 1 : -1,
		rayDir.z > 0 ? 1 : -1
	);

	// Calculate initial boundary distances
	glm::vec3 tMax(
		(voxelX + (step.x > 0) - currentRayPos.x) / rayDir.x,
		(voxelY + (step.y > 0) - currentRayPos.y) / rayDir.y,
		(voxelZ + (step.z > 0) - currentRayPos.z) / rayDir.z
	);

	// Calculate the distance to the next boundary in each axis
	glm::vec3 tDelta(
		VoxelSize / std::abs(rayDir.x),
		VoxelSize / std::abs(rayDir.y),
		VoxelSize / std::abs(rayDir.z)
	);

	for (float t = 0.0f; t < maxDistance;)
	{
		// Calculate the chunk coordinates for the current voxel
		auto chunk = GetChunkCoordinates(currentRayPos);

		// Calculate the local voxel coordinates within the chunk
		int localX = voxelX - chunk.x * ChunkSize;
		int localY = voxelY - chunk.y * ChunkSize;
		int localZ = voxelZ - chunk.z * ChunkSize;

		// Check if the voxel is within the chunk bounds
		if (localX >= 0 && localX < ChunkSize &&
			localY >= 0 && localY < ChunkSize &&
			localZ >= 0 && localZ < ChunkSize)
		{
			auto chunkPtr = GetChunkByChunkPos(chunk);
			if (chunkPtr && chunkPtr->GetBlock(localX, localY, localZ).GetType() != vxlBlock::VoxelType::Air)
			{
				hit.hit = true;
				hit.hitPosition = glm::vec3(voxelX, voxelY, voxelZ);
				hit.distance = t;
				hit.voxelCoords = glm::ivec3(localX, localY, localZ);

				// Determine the normal of the hit face based on the step direction and tMax values
				if (tMax.x < tMax.y && tMax.x < tMax.z) {
					hit.hitNormal = glm::vec3(step.x > 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
				}
				else if (tMax.y < tMax.z) {
					hit.hitNormal = glm::vec3(0.0f, step.y > 0 ? -1.0f : 1.0f, 0.0f);
				}
				else {
					hit.hitNormal = glm::vec3(0.0f, 0.0f, step.z > 0 ? -1.0f : 1.0f);
				}

				return hit;
			}
		}

		// Move to the next voxel boundary
		if (tMax.x < tMax.y && tMax.x < tMax.z)
		{
			t = tMax.x;
			tMax.x += tDelta.x;
			voxelX += step.x;
		}
		else if (tMax.y < tMax.z)
		{
			t = tMax.y;
			tMax.y += tDelta.y;
			voxelY += step.y;
		}
		else
		{
			t = tMax.z;
			tMax.z += tDelta.z;
			voxelZ += step.z;
		}

		// Incrementally update current ray position to avoid floating-point errors
		currentRayPos = ray.origin + rayDir * t;
	}

	// If no intersection was found, return an invalid hit
	return hit;
}