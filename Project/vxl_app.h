#pragma once
#include "vxl_Device.h"
#include "vxl_GraphicsPipeline.h"
#include "vxl_SwapChain.h"
#include "vxl_Model.h"
#include "vxl_Window.h"

#include <memory>
#include <vector>

#include "vxl_Camera.h"
#include "vxl_Chunk.h"
#include "vxl_GraphicsPipeline3D.h"
#include "vxl_Model3D.h"
#include "vxl_UBO.h"
#include "vxl_World.h"

namespace vxl
{
	class vxlApp
	{
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;
	public:
		void PrintControls();
		vxlApp();
		~vxlApp();

		vxlApp(const vxlApp& other) = delete;
		vxlApp(vxlApp&& other) = delete;
		vxlApp& operator=(const vxlApp& other) = delete;
		vxlApp& operator=(vxlApp&& other) = delete;

		void Run();
	private:

		void LoadModels();

		void CreateUBO();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIdx);


		vxlWindow m_vxlWindow{ WIDTH, HEIGHT, "vxlEngine" };
		vxlCamera m_vxlCamera{ m_vxlWindow };
		vxlDevice m_vxlDevice{ m_vxlWindow };

		std::unique_ptr<vxlSwapChain> m_vxlSwapChain{ nullptr };
		std::unique_ptr<vxlGraphicsPipeline> m_vxlGraphicsPipeline{ nullptr };
		std::unique_ptr<vxlGraphicsPipeline3D> m_vxlGraphicsPipeline3D{ nullptr };
		
		

		VkPipelineLayout m_pipelineLayout;
		VkPipelineLayout m_pipelineLayout3D;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<vxlModel> m_vxlHotbarModel{ nullptr };
		std::unique_ptr<vxlModel> m_vxlCrosshairModel{ nullptr };
		std::unique_ptr<vxlModel3D> m_vxlModel3D{ nullptr };
		std::unique_ptr<vxlWorld> m_vxlWorld{};

		std::unique_ptr<vxlUBO> m_vxlUBO;
	};
}
