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
			Solid,
			Stone,
			Dirt,
			Grass,
			Sand,

		};
	public:
		vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type, uint32_t id);
		vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type, const glm::vec<2, uint32_t>& rowCol);
		~vxlBlock() = default;

		/*vxlBlock(const vxlBlock& other) = delete;
		vxlBlock(vxlBlock&& other) = delete;*/
		//vxlBlock& operator=(const vxlBlock& other) = delete;
		//vxlBlock& operator=(vxlBlock&& other) = delete;

		void SetType(VoxelType type) { m_type = type; }
		void SetID(uint32_t id) { m_id = id; }
		void SetID(const glm::vec<2, uint32_t>& rowCol) { m_id = rowCol.x * 32 + rowCol.y; }
		void SetPosition(const glm::vec3& pos) { m_position = pos; }


		VoxelType GetType() const { return m_type; }
		glm::vec3  GetPosition() const { return m_position; }
		glm::vec4 GetColor() const { return m_color; }
		uint32_t GetID() const { return m_id; }
	private:
		uint32_t m_id{ 0 };
		glm::vec3 m_position; // World coordinates
		glm::vec4 m_color;    // RGBA color
		VoxelType m_type;     // Type of voxel
	};
}

