#pragma once
#include <glm/glm.hpp>
namespace vxl
{
	constexpr uint32_t RowColToIdx(const glm::vec<2, uint32_t>& rowCol, int gridsize = 32)
	{
		return rowCol.x * gridsize + rowCol.y;
	}
	class vxlBlock
	{
	public:
		enum class VoxelType : uint32_t
		{
			Air = 0,
			Stone = RowColToIdx({26,6}),
			Dirt = RowColToIdx({13,21}),
			GrassSide = RowColToIdx({8,25}),
			GrassTop = RowColToIdx({10,26}),
			Sand = RowColToIdx({23,16}),
			TEST_BLOCK = RowColToIdx({0,20}),
			OakPlanks = RowColToIdx({ 18,7 }),
			OakLog = RowColToIdx({18,5}),
			OakLeaves = RowColToIdx({18,4}),
			CobbleStone = RowColToIdx({14,13}),
			Bricks = RowColToIdx({9,9}),
			Glass = RowColToIdx({15,24}),


		};
	public:
		vxlBlock(const glm::vec3& worldPosition, VoxelType type);
		~vxlBlock() = default; 

		/*vxlBlock(const vxlBlock& other) = delete;
		vxlBlock(vxlBlock&& other) = delete;*/
		//vxlBlock& operator=(const vxlBlock& other) = delete;
		//vxlBlock& operator=(vxlBlock&& other) = delete;

		void SetType(VoxelType type) { m_type = type; }
		void SetPosition(const glm::vec3& pos) { m_position = pos; }


		VoxelType GetType() const { return m_type; }
		glm::vec3  GetPosition() const { return m_position; }
		
	private:
		uint32_t Vec2ToId(const glm::ivec2& rowCol) const;

		const int AtlasSize{ 32 };
		glm::vec3 m_position; // World coordinates
		VoxelType m_type;     // Type of voxel
	};
}

