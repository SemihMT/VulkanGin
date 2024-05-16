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
	class vxlModel3D
	{
	public:
		struct Vertex3D
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

	public:
		vxlModel3D(vxlDevice& device, const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices);
		~vxlModel3D();


		vxlModel3D(const vxlModel3D& other) = delete;
		vxlModel3D(vxlModel3D&& other) = delete;
		vxlModel3D& operator=(const vxlModel3D& other) = delete;
		vxlModel3D& operator=(vxlModel3D&& other) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
	private:

		void CreateVertexBuffers(const std::vector<Vertex3D>& vertices);
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
