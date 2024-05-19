#include "vxl_Block.h"

vxl::vxlBlock::vxlBlock(const glm::vec3& worldPosition, glm::vec4 color, VoxelType type) :
	position{ worldPosition },
	color{ color },
	type{ type }
{
}
