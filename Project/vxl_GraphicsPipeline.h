#pragma once
#include "vxl_Device.h"
#include <string>
#include <vector>

#include "vxl_Utils.h"

namespace vxl
{
	
	class vxlGraphicsPipeline
	{
	public:
		vxlGraphicsPipeline(vxlDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfiguration& config);
		~vxlGraphicsPipeline();


		vxlGraphicsPipeline(const vxlGraphicsPipeline& other) = delete;
		vxlGraphicsPipeline(vxlGraphicsPipeline&& other) = delete;
		vxlGraphicsPipeline& operator=(const vxlGraphicsPipeline& other) = delete;
		vxlGraphicsPipeline& operator=(vxlGraphicsPipeline&& other) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		// This function fills in a struct that contains most of the pipeline information we need to get set up for 2D rendering
		// The user should change whatever settings they see fit to make the pipeline work in the way they want
		static void CreateDefaultPipelineConfiguration(PipelineConfiguration& config);

	private:

		static std::vector<char> ReadFile(const std::string& filePath);
		void CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfiguration& config);
		void CreateShaderModule(const std::vector<char>& shaderCode, VkShaderModule* shaderModule);
		//Reference allowed here as the pipeline depends entirely on the device existing
		vxlDevice& m_device;
		VkPipeline m_graphicsPipeline;
		//Add more if implementing more pipeline stages
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;

	};
}
