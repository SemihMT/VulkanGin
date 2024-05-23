#include "vxl_Block.h"
#include <glm/gtc/noise.hpp>

vxl::vxlBlock::vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type, uint32_t id) :
	m_id{ id },
	m_position{ worldPosition },
	m_color{ color },
	m_type{ type }
{
}

vxl::vxlBlock::vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type, const glm::vec<2, uint32_t>& rowCol):
	m_id{ rowCol.x * 32 + rowCol.y},
	m_position{ worldPosition },
	m_color{ color },
	m_type{ type }
{
	
}
