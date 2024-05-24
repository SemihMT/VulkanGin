#include "vxl_Block.h"
#include <glm/gtc/noise.hpp>

vxl::vxlBlock::vxlBlock(const glm::vec3& worldPosition, VoxelType type) :
	m_position{ worldPosition },
	m_type{type}
{
}

uint32_t vxl::vxlBlock::Vec2ToId(const glm::ivec2& rowCol) const
{
	return rowCol.x * AtlasSize + rowCol.y;
}
