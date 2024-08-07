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


int vxl::GetSquaredDistance(const glm::ivec3& pos1, const glm::ivec3& pos2)
{
	const int dx = pos1.x - pos2.x;
	const int dy = pos1.y - pos2.y;
	const int dz = pos1.z - pos2.z;
	return dx * dx + dy * dy + dz * dz;
}

float vxl::GenerateLayeredNoise(const glm::vec3& pos, int octaves, float persistence, float lacunarity, float scale)
{
	float noiseValue = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	float maxValue = 0.0f;

	for (int i = 0; i < octaves; ++i)
	{
		noiseValue += glm::simplex(pos * frequency * scale) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= lacunarity;
	}

	noiseValue /= maxValue;
	return (noiseValue + 1.0f) / 2.0f; // Normalize to [0, 1]
}

float vxl::GenerateLayeredNoise(const glm::vec2& pos, int octaves, float persistence, float lacunarity, float scale)
{
	float noiseValue = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;
	float maxValue = 0.0f;

	for (int i = 0; i < octaves; ++i)
	{
		noiseValue += glm::simplex(pos * frequency * scale) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= lacunarity;
	}

	noiseValue /= maxValue;
	return (noiseValue + 1.0f) / 2.0f; // Normalize to [0, 1]
}

//Helper function for stuff that will get rendered in 2d as these will get passed to the renderer using ndc/screen space coords
glm::vec2 vxl::NDCToScreenSpace(const glm::vec2& ndc, int width, int height)
{
	float xScreen = (ndc.x + 1.0f) * 0.5f * width;
	float yScreen = (1.0f - (ndc.y + 1.0f) * 0.5f) * height;
	return { xScreen, yScreen };
}

glm::vec2 vxl::ScreenSpaceToNDC(const glm::vec2& screen, int width, int height)
{
	float xNdc = (2.0f * screen.x) / width - 1.0f;
	float yNdc = 1.0f - (2.0f * screen.y) / height;
	return { xNdc, yNdc };
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
