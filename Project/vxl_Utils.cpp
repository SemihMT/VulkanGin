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
