#pragma once
#include "vxl_Device.h"
#include <string>
#include <vector>

#include "vxl_Utils.h"

namespace vxl
{
	
	class vxlGraphicsPipeline3D
	{
	public:
		vxlGraphicsPipeline3D(vxlDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfiguration& config);
		~vxlGraphicsPipeline3D();

		
		vxlGraphicsPipeline3D(const vxlGraphicsPipeline3D& other) = delete;
		vxlGraphicsPipeline3D(vxlGraphicsPipeline3D&& other) = delete;
		vxlGraphicsPipeline3D& operator=(const vxlGraphicsPipeline3D& other) = delete;
		vxlGraphicsPipeline3D& operator=(vxlGraphicsPipeline3D&& other) = delete;

		void Bind(VkCommandBuffer commandBuffer);
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
