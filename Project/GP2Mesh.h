#pragma once
#include <vector>

#include "GP2CommandPool.h"
#include "GP2IndexBuffer.h"
#include "GP2VertexBuffer.h"

template <typename VertexType>
class GP2Mesh
{
public:
	GP2Mesh(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool,
		const std::vector<VertexType>& vertices, const std::vector<uint16_t>& indices)
	{
		Initialize(device, physicalDevice, commandPool, vertices, indices);
	}

	GP2Mesh(VkDevice device, VkPhysicalDevice physicalDevice, GP2CommandPool commandPool, const std::string& pathToModel)
	{
		Initialize(device, physicalDevice, commandPool, pathToModel);
	}

	~GP2Mesh() = default;

	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, const GP2CommandPool& commandPool,
		const std::vector<VertexType>& vertices, const std::vector<uint16_t>& indices)
	{
		m_commandPool = commandPool;
		m_vertices = vertices;
		m_indices = indices;

		m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(m_vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(m_indices);
	}


	void Initialize(VkDevice device, VkPhysicalDevice physicalDevice, const GP2CommandPool& commandPool, const std::string& pathToModel)
	{
		m_commandPool = commandPool;

		if(!parseOBJ(pathToModel))
		{
			throw std::runtime_error{ "Error parsing the obj!" };
		}

		m_vertexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_vertexBuffer.CreateVertexBuffer(m_vertices);

		m_indexBuffer.Initialize(device, physicalDevice, m_commandPool);
		m_indexBuffer.CreateIndexBuffer(m_indices);
	}

	void Destroy()
	{
		m_vertexBuffer.Destroy();
		m_indexBuffer.Destroy();
	}

	void Draw(const GP2CommandBuffer& cmdBuffer) const
	{
		vkCmdDrawIndexed(cmdBuffer.GetVkCommandBuffer(), static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);
	}

	GP2VertexBuffer<VertexType> GetVertexBuffer() const { return m_vertexBuffer; }
	GP2IndexBuffer GetIndexBuffer() const { return m_indexBuffer; }

	void Bind(const GP2CommandBuffer& cmdBuffer) const
	{
		VkBuffer buffers[1]{ m_vertexBuffer.GetBuffer().GetVkBuffer() };
		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(cmdBuffer.GetVkCommandBuffer(), 0, 1, buffers, &offset);

		vkCmdBindIndexBuffer(cmdBuffer.GetVkCommandBuffer(), m_indexBuffer.GetBuffer().GetVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
	}

	void Translate(const glm::vec3& translation)
	{
		
	}

private:
	bool parseOBJ(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file)
			return false;

		std::string sCommand;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
	
		std::vector<uint16_t> indices;

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
				positions.push_back({ x, y, z });
			}
			else if (sCommand == "f")
			{
				uint16_t i0, i1, i2;
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


		std::vector vertexNormals(positions.size(), glm::vec3(0.0f));
		//If the obj file did not contain any normal information
		if (normals.empty())
		{
			
			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				glm::vec3 edgeV0V1 = positions[i1] - positions[i0];
				glm::vec3 edgeV0V2 = positions[i2] - positions[i0];
				glm::vec3 normal = glm::cross(edgeV0V1, edgeV0V2);

				
				normal = glm::normalize(normal);

				// Accumulate face normal to vertex normals
				vertexNormals[i0] += normal;
				vertexNormals[i1] += normal;
				vertexNormals[i2] += normal;
			}

			// Normalize vertex normals
			for (uint32_t i = 0; i < vertexNormals.size(); ++i) {
				vertexNormals[i] = glm::normalize(vertexNormals[i]);
			}
		}

		/*if (positions.size() != normals.size())
		{
			throw std::runtime_error{ "the number of vertices and normals do not match!" };
		}*/


		m_vertices.resize(positions.size());
		m_indices.resize(indices.size());
		for (int i{}; i < positions.size(); ++i)
		{
			m_vertices[i] = VertexType{ positions[i], {0.5f,0.5f,0.5f}, vertexNormals[i] };
		}
		for (int i{}; i < indices.size(); ++i)
		{
			m_indices[i] = indices[i];
		}

		return true;
	}



	GP2CommandPool m_commandPool{};
	GP2CommandBuffer m_commandBuffer{};

	GP2VertexBuffer<VertexType> m_vertexBuffer{};
	GP2IndexBuffer m_indexBuffer{};

	std::vector<VertexType> m_vertices{};
	std::vector<uint16_t> m_indices{};
};

