#include "vxl_World.h"

#include <execution>
#include <iostream>

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
		[&chunkPosition](const std::unique_ptr<vxlChunk>& chunk)
		{
			return chunk->GetChunkPos() == chunkPosition;
		}
	);

	if (it != m_loadedChunks.end())
	{
		return it->get();
	}
	return nullptr;
}

void vxl::vxlWorld::OnMouseButton(int button, int action, int mods)
{
	if (action == GLFW_RELEASE)
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
				if (localPlacementCoords.x < 0) //The block we'll place will be in the chunk LEFT of the chunk of the block we hit
				{
					chunkOffset.x -= 1;
					localPlacementCoords.x = vxlChunk::ChunkSize - 1;
				}
				else if (localPlacementCoords.x >= vxlChunk::ChunkSize)
				{
					chunkOffset.x += 1;
					localPlacementCoords.x = 0;
				}

				if (localPlacementCoords.y < 0) //The block we'll place will be in the chunk UNDER the chunk of the block we hit 
				{
					chunkOffset.y -= 1;
					localPlacementCoords.y = vxlChunk::ChunkSize - 1;
				}
				else if (localPlacementCoords.y >= vxlChunk::ChunkSize)
				{
					chunkOffset.y += 1;
					localPlacementCoords.y = 0;
				}

				if (localPlacementCoords.z < 0)  //The block we'll place will be in the chunk BEHIND the chunk of the block we hit 
				{
					chunkOffset.z -= 1;
					localPlacementCoords.z = vxlChunk::ChunkSize - 1;
				}
				else if (localPlacementCoords.z >= vxlChunk::ChunkSize)
				{
					chunkOffset.z += 1;
					localPlacementCoords.z = 0;
				}

				// Get the chunk where the block will be placed
				auto placedBlockChunk = GetChunkByChunkPos(hitChunk->GetChunkPos() + chunkOffset);

				// Place the new block in the calculated position
				if (placedBlockChunk)
				{
					if (placedBlockChunk->GetBlock(localPlacementCoords.x, localPlacementCoords.y, localPlacementCoords.z).GetType() == vxlBlock::VoxelType::Air)
					{
						// Place the new block in the calculated position if it's not occupied
						placedBlockChunk->PlaceBlock(localPlacementCoords, m_currentSelectedBlock);
					}
				}
			}

		}
	}

}

void vxl::vxlWorld::OnKeyPress(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::Stone;
		std::cout << "Current Block: Stone\n";
	}
	if (key == GLFW_KEY_2)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::CobbleStone;
		std::cout << "Current Block: CobbleStone\n";
	}
	if (key == GLFW_KEY_3)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::Bricks;
		std::cout << "Current Block: Bricks\n";
	}
	if (key == GLFW_KEY_4)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::Dirt;
		std::cout << "Current Block: Dirt\n";
	}
	if (key == GLFW_KEY_5)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::OakPlanks;
		std::cout << "Current Block: Oak Planks\n";
	}
	if (key == GLFW_KEY_6)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::OakLog;
		std::cout << "Current Block: Oak Log\n";
	}
	if (key == GLFW_KEY_7)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::OakLeaves;
		std::cout << "Current Block: Oak Leaves\n";
	}
	if (key == GLFW_KEY_8)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::Glass;
		std::cout << "Current Block: Glass\n";
	}
	if (key == GLFW_KEY_9)
	{
		m_currentSelectedBlock = vxlBlock::VoxelType::Sand;
		std::cout << "Current Block: Sand\n";
	}
}

void vxl::vxlWorld::Draw(VkCommandBuffer commandBuffer)
{
	for (const auto& chunk : m_loadedChunks)
	{
		glm::vec3 cameraPos{ m_camera->GetPosition() };
		glm::ivec3 currentChunkPos{ GetChunkCoordinates(cameraPos) };
		glm::ivec3 chunkPos = chunk->GetChunkPos();

		const int squaredDistance = GetSquaredDistance(chunkPos, currentChunkPos);

		if (squaredDistance <= m_viewRadius * m_viewRadius)
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
	// Calculate current chunk position
	glm::ivec3 currentChunkPos = GetChunkCoordinates(cameraPos);


	GenerateChunksWithinRadius(currentChunkPos, m_updateRadius);

	RemoveChunksBeyondRadius(currentChunkPos, m_removeRadius);

	// Ensure GPU has finished work
	vkQueueWaitIdle(m_device.GetGraphicsQueue());
}



void vxl::vxlWorld::Init()
{
	// Get camera position
	glm::vec3 cameraPos = m_camera->GetPosition();

	// Calculate current chunk position
	glm::ivec3 currentChunkPos = GetChunkCoordinates(cameraPos);

	GenerateChunksWithinRadius(currentChunkPos, m_initRadius);
}

void vxl::vxlWorld::GenerateChunk(const glm::ivec3& chunkPos)
{
	m_loadedChunks.push_back(std::make_unique<vxlChunk>(m_device, chunkPos, m_seed));
}

void vxl::vxlWorld::GenerateChunk(const glm::vec3& worldPosition)
{
	GenerateChunk(GetChunkCoordinates(worldPosition));
}

void vxl::vxlWorld::GenerateChunksWithinRadius(const glm::ivec3& currentChunkPos, int radius)
{
	for (int x = currentChunkPos.x - radius; x <= currentChunkPos.x + radius; ++x)
	{
		for (int y = currentChunkPos.y - radius; y <= currentChunkPos.y + radius; ++y)
		{
			for (int z = currentChunkPos.z - radius; z <= currentChunkPos.z + radius; ++z)
			{
				glm::ivec3 chunkPos{ x, y, z };
				const int distanceSquared = GetSquaredDistance(chunkPos, currentChunkPos);

				if (GetChunkByChunkPos(chunkPos) == nullptr && distanceSquared <= radius * radius)
				{
					GenerateChunk(chunkPos);
				}
			}
		}
	}
}

void vxl::vxlWorld::RemoveChunksBeyondRadius(const glm::ivec3& currentChunkPos, int radius)
{
	auto it = m_loadedChunks.begin();
	while (it != m_loadedChunks.end())
	{
		glm::ivec3 chunkPos = (*it)->GetChunkPos();
		const int distanceSquared = GetSquaredDistance(chunkPos, currentChunkPos);

		if (distanceSquared > radius * radius)
		{
			it = m_loadedChunks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

vxl::RaycastHit vxl::vxlWorld::PerformRaycast(const vxl::Ray& ray, float maxDistance)
{
	// Init the returned struct
	RaycastHit hitInfo{};
	hitInfo.distance = maxDistance;

	// Extract the starting position and direction from the ray
	glm::vec3 pos = ray.origin;
	glm::vec3 dir = ray.direction;

	// Determine the step direction for each axis based on the ray direction
	int stepX = (dir.x > 0) ? 1 : -1;
	int stepY = (dir.y > 0) ? 1 : -1;
	int stepZ = (dir.z > 0) ? 1 : -1;

	// Calculate the initial maximum t values for each axis
	// These values are the initial distances to the next voxel boundary from 'pos'
	float tMaxX = (dir.x > 0) ? (std::ceil(pos.x) - pos.x) / dir.x : (pos.x - std::floor(pos.x)) / -dir.x;
	float tMaxY = (dir.y > 0) ? (std::ceil(pos.y) - pos.y) / dir.y : (pos.y - std::floor(pos.y)) / -dir.y;
	float tMaxZ = (dir.z > 0) ? (std::ceil(pos.z) - pos.z) / dir.z : (pos.z - std::floor(pos.z)) / -dir.z;

	// Calculate the t value increments for each axis
	float tDeltaX = std::abs(1.0f / dir.x);
	float tDeltaY = std::abs(1.0f / dir.y);
	float tDeltaZ = std::abs(1.0f / dir.z);

	// Iterate until the maximum distance is reached or a hit is detected
	while (hitInfo.distance > 0)
	{
		// Determine which axis to step along next
		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				// Take a step along the X axis
				pos.x += stepX;
				hitInfo.distance = tMaxX;
				tMaxX += tDeltaX;
				hitInfo.hitNormal = { -stepX, 0, 0 };
			}
			else
			{
				// Take a step along the Z axis
				pos.z += stepZ;
				hitInfo.distance = tMaxZ;
				tMaxZ += tDeltaZ;
				hitInfo.hitNormal = { 0, 0, -stepZ };
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				// Take a step along the Y axis
				pos.y += stepY;
				hitInfo.distance = tMaxY;
				tMaxY += tDeltaY;
				hitInfo.hitNormal = { 0, -stepY, 0 };
			}
			else
			{
				// Take a step along the Z axis
				pos.z += stepZ;
				hitInfo.distance = tMaxZ;
				tMaxZ += tDeltaZ;
				hitInfo.hitNormal = { 0, 0, -stepZ };
			}
		}

		// Check if the current voxel is occupied (not air)
		if (GetBlock(pos)->GetType() != vxlBlock::VoxelType::Air)
		{
			hitInfo.hit = true;
			hitInfo.hitPosition = pos;

			// Calculate the local position within the chunk
			int localX = static_cast<int>(std::floor(pos.x)) % vxlChunk::ChunkSize;
			int localY = static_cast<int>(std::floor(pos.y)) % vxlChunk::ChunkSize;
			int localZ = static_cast<int>(std::floor(pos.z)) % vxlChunk::ChunkSize;

			// Handle negative modulo results to ensure correct local coordinates
			if (localX < 0) localX += vxlChunk::ChunkSize;
			if (localY < 0) localY += vxlChunk::ChunkSize;
			if (localZ < 0) localZ += vxlChunk::ChunkSize;


			hitInfo.voxelCoords = { localX,localY,localZ };

			// Print debugging information about the hit
			std::cout << "Raycast Hit Detected!\n"
				<< "Ray Origin: (" << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z << ")\n"
				<< "Ray Direction: (" << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << ")\n"
				<< "Hit Position: (" << hitInfo.hitPosition.x << ", " << hitInfo.hitPosition.y << ", " << hitInfo.hitPosition.z << ")\n"
				<< "Hit Normal: (" << hitInfo.hitNormal.x << ", " << hitInfo.hitNormal.y << ", " << hitInfo.hitNormal.z << ")\n"
				<< "Distance: " << hitInfo.distance << "\n"
				<< "Voxel Coords: (" << localX << ", " << localY << ", " << localZ << ")\n";
			return hitInfo;
		}

		// If the distance exceeds the maximum, exit the loop
		if (hitInfo.distance > maxDistance)
		{
			hitInfo.distance = maxDistance;
			break;
		}
	}

	// If no voxel was hit, return default RaycastHit (missed)
	return hitInfo;
}

vxl::vxlBlock* vxl::vxlWorld::GetBlock(const glm::vec3& worldPosition)
{
	// Get the chunk containing the given world position
	vxlChunk* chunk = GetChunkByWorldPos(worldPosition);
	if (!chunk)
	{
		return nullptr; // No chunk at this position
	}

	// Calculate the local position within the chunk
	int localX = static_cast<int>(std::floor(worldPosition.x)) % vxlChunk::ChunkSize;
	int localY = static_cast<int>(std::floor(worldPosition.y)) % vxlChunk::ChunkSize;
	int localZ = static_cast<int>(std::floor(worldPosition.z)) % vxlChunk::ChunkSize;

	// Handle negative modulo results to ensure correct local coordinates
	if (localX < 0) localX += vxlChunk::ChunkSize;
	if (localY < 0) localY += vxlChunk::ChunkSize;
	if (localZ < 0) localZ += vxlChunk::ChunkSize;

	// Retrieve the block from the chunk
	return &chunk->GetBlock(localX, localY, localZ);
}
