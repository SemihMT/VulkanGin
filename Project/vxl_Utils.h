#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan_core.h>

#include "vxl_Block.h"
#include "vxl_Chunk.h"

namespace vxl
{
	class vxlChunk;
	constexpr int MAX_FRAMES_IN_FLIGHT = 2;



	/// <summary>
	/// Parses an OBJ file and extracts its geometric data.
	/// </summary>
	/// <param name="filename">The path to the OBJ file to parse.</param>
	/// <param name="indices">The output vector of triangle indices.</param>
	/// <param name="positions">The output vector of vertex positions.</param>
	/// <param name="normals">The output vector of vertex normals.</param>
	/// <returns>true if the parsing was successful, false otherwise.</returns>
	bool parseOBJ(const std::string& filename, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals);

	//struct that will store configuration info so that the application layer code can easily reconfigure/recreate a pipeline
	struct PipelineConfiguration
	{
		PipelineConfiguration() = default;
		//Prevent copying
		PipelineConfiguration(const PipelineConfiguration&) = delete;
		PipelineConfiguration& operator=(const PipelineConfiguration&) = delete;


		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;

		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	glm::vec2 NDCToScreenSpace(const glm::vec2& ndc, int width, int height);

	glm::vec2 ScreenSpaceToNDC(const glm::vec2& screen, int width, int height);

	vxlChunk* GetChunk(const glm::vec3& worldPos);

	glm::ivec3 GetChunkCoordinates(const glm::vec3& worldPos, int chunkSize = vxlChunk::ChunkSize);

	glm::ivec3 GetVoxelCoords(const glm::vec3& worldPos, const vxlChunk& chunk);

	struct RaycastHit
	{
		bool hit;                   // Indicates whether the ray hit something
		glm::vec3 hitPosition;      // The exact position where the ray hit
		glm::vec3 hitNormal;        // The normal of the surface at the hit point
		int voxelId;                // The ID of the voxel that was hit
		glm::ivec3 voxelCoords;     // The coordinates of the voxel in the chunk
		float distance;             // The distance from the ray origin to the hit point

		// Default constructor for when no hit occurs
		RaycastHit() : hit(false), hitPosition(0.0f), hitNormal(0.0f), voxelId(-1), voxelCoords(0), distance(0.0f) {}
	};

	struct Ray
	{
		glm::vec3 origin;
		glm::vec3 direction;
	};
	constexpr float RaycastStepSize = 0.1f;
	RaycastHit PerformRaycast(const Ray& ray, float maxDistance = 100.0f);
}
