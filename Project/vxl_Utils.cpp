#include "vxl_Utils.h"

bool vxl::parseOBJ(const std::string& filename, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions,
	std::vector<glm::vec3>& normals)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::string sCommand;

	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;
			positions.emplace_back(x, y, z);
		}
		else if (sCommand == "f")
		{
			uint32_t i0, i1, i2;
			file >> i0 >> i1 >> i2;

			indices.push_back(i0 - 1);
			indices.push_back(i1 - 1);
			indices.push_back(i2 - 1);
		}
		else if (sCommand == "vn")
		{
			float x, y, z;
			file >> x >> y >> z;
			glm::vec3 normal{ x,y,z };
			normals.push_back(glm::normalize(normal));
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');

		if (file.eof())
			break;
	}
	return false;
}

//Helper function for stuff that will get rendered in 2d as these will get passed to the renderer using ndc/screen space coords
glm::vec2 vxl::NDCToScreenSpace(const glm::vec2& ndc, int width, int height)
{
	float xScreen = (ndc.x + 1.0f) * 0.5f * width;
	float yScreen = (1.0f - (ndc.y + 1.0f) * 0.5f) * height;
	return {xScreen, yScreen};
}

glm::vec2 vxl::ScreenSpaceToNDC(const glm::vec2& screen, int width, int height)
{
	float xNdc = (2.0f * screen.x) / width - 1.0f;
	float yNdc = 1.0f - (2.0f * screen.y) / height;
	return {xNdc, yNdc};
}

glm::ivec3 vxl::GetChunkCoordinates(const glm::vec3& worldPos, int chunkSize)
{
	glm::ivec3 chunkCoord;
	chunkCoord.x = static_cast<int>(std::floor(worldPos.x / chunkSize));
	chunkCoord.y = static_cast<int>(std::floor(worldPos.y / chunkSize));
	chunkCoord.z = static_cast<int>(std::floor(worldPos.z / chunkSize));
	return chunkCoord;
}

glm::ivec3 vxl::GetVoxelCoords(const glm::vec3& worldPos, const vxlChunk& chunk)
{/*
	glm::vec3 relativePos = worldPos - chunk.GetWorldPosition();

	return glm::ivec3(
		static_cast<int>(floor(relativePos.x)),
		static_cast<int>(floor(relativePos.y)),
		static_cast<int>(floor(relativePos.z))
	);*/
	return {};
}

vxl::RaycastHit vxl::PerformRaycast(const Ray& ray, float maxDistance)
{
	//RaycastHit hit{};
	//for (float t = 0.0f; t < maxDistance; t += RaycastStepSize)
	//{
	//	const glm::vec3 currentRayPos = ray.origin + ray.direction * t;

	//	// Calculate the chunk coordinates from the ray position
	//	const int chunkX = static_cast<int>(std::floor(currentRayPos.x / 16.0f));
	//	const int chunkY = static_cast<int>(std::floor(currentRayPos.y / 16.0f));
	//	const int chunkZ = static_cast<int>(std::floor(currentRayPos.z / 16.0f));

	//	// Calculate the local voxel coordinates within the chunk
	//	int voxelX = static_cast<int>(std::floor(currentRayPos.x - chunkX * 16.0f));
	//	int voxelY = static_cast<int>(std::floor(currentRayPos.y - chunkY * 16.0f));
	//	int voxelZ = static_cast<int>(std::floor(currentRayPos.z - chunkZ * 16.0f));

	//	// Check if the ray intersects with a voxel in the chunk
	//	if (voxelX >= 0 && voxelX < 16 && voxelY >= 0 && voxelY < 16 && voxelZ >= 0 && voxelZ < 16) {
	//		// Get the chunk and voxel data
	//		vxlChunk* chunk = GetChunk(chunkX, chunkY, chunkZ);
	//		vxlBlock* voxel = chunk->GetBlock(voxelX, voxelY, voxelZ);

	//		// If the voxel is solid, calculate the hit information and exit
	//		if (voxel->GetType() == vxlBlock::VoxelType::Solid) {
	//			hit.hit = true;
	//			hit.hitPosition = glm::vec3(chunkX * 16 + voxelX, chunkY * 16 + voxelY, chunkZ * 16 + voxelZ);
	//			hit.distance = t;
	//			hit.hitNormal = CalculateFaceNormal(ray.direction, voxelX, voxelY, voxelZ);
	//			hit.voxelId = voxel->GetID();
	//			hit.voxelCoords = glm::ivec3{ voxelX,voxelY,voxelZ };
	//			return hit;
	//		}
	//	}

	//}
	//// If no intersection was found, return an invalid hit
	//return hit;
	return {};
}
