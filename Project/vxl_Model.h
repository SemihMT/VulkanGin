#pragma once
#include "vxl_Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //Force depth values to range 0.0 to 1.0 instead of -1.0 to 1.0 (opengl)
#include <glm/glm.hpp>

#include <vector>

namespace vxl
{
	//Model class that handles mesh creation on the cpu
	//Handles sending this data to the gpu via buffers
	class vxlModel
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 color;
			glm::vec2 texCoord;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

	public:
		vxlModel(vxlDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~vxlModel();


		vxlModel(const vxlModel& other) = delete;
		vxlModel(vxlModel&& other) = delete;
		vxlModel& operator=(const vxlModel& other) = delete;
		vxlModel& operator=(vxlModel&& other) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
	private:

		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		vxlDevice& m_device;

		//buffer
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;

		uint32_t m_vertexCount;
		uint32_t m_indexCount;

	};
}
