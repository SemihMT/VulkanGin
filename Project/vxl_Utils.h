#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan_core.h>
namespace vxl
{
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
}
