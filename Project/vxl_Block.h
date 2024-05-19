#pragma once
#include <glm/glm.hpp>
namespace vxl
{

	class vxlBlock
	{
	public:
		enum class VoxelType
		{
			Air,
			Solid
		};
	public:
		vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type);
		~vxlBlock() = default;

		/*vxlBlock(const vxlBlock& other) = delete;
		vxlBlock(vxlBlock&& other) = delete;*/
		//vxlBlock& operator=(const vxlBlock& other) = delete;
		//vxlBlock& operator=(vxlBlock&& other) = delete;


		VoxelType GetType() const { return type; }
		glm::vec3  GetPosition() const { return position; }
		glm::vec4 GetColor() const { return color; }
	private:
		glm::vec3 position; // World coordinates
		glm::vec4 color;    // RGBA color
		VoxelType type;     // Type of voxel
	};
}

