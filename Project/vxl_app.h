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
		vxlApp();
		~vxlApp();

		vxlApp(const vxlApp& other) = delete;
		vxlApp(vxlApp&& other) = delete;
		vxlApp& operator=(const vxlApp& other) = delete;
		vxlApp& operator=(vxlApp&& other) = delete;

		void Run();								// Start while loop
	private:
		void PrintControls();					// Print the controls to the console

		void LoadModels();						// Create 2D meshes (UI) and initialize the 3D world
		void CreateUBO();						// Create the UBO to pass program variables to the shaders
		void CreatePipelineLayout();			// Create the 2D and 3D pipeline layouts 
		void CreatePipeline();					// Create the pipelines from the layouts
		void CreateCommandBuffers();			// Create the command buffers to process GPU commands
		void FreeCommandBuffers();				// Free/cleanup the command buffers
		void DrawFrame();						// Calls 'RecordCommandBuffer' and submits the recorded buffer (also some swap chain stuff)
		void RecreateSwapChain();				// Recreate the swap chain when the format changes (e.g. when the screen resizes)
		void RecordCommandBuffer(int imageIdx);	// Send the Draw commands to the GPU through the command buffers




		vxlWindow m_vxlWindow{ WIDTH, HEIGHT, "vxlEngine" };
		vxlCamera m_vxlCamera{ m_vxlWindow };
		vxlDevice m_vxlDevice{ m_vxlWindow };

		std::unique_ptr<vxlSwapChain> m_vxlSwapChain{ nullptr };
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<vxlUBO> m_vxlUBO;

		VkPipelineLayout m_pipelineLayout;
		std::unique_ptr<vxlGraphicsPipeline> m_vxlGraphicsPipeline{ nullptr };

		VkPipelineLayout m_pipelineLayout3D;
		std::unique_ptr<vxlGraphicsPipeline3D> m_vxlGraphicsPipeline3D{ nullptr };


		//2D meshes
		std::unique_ptr<vxlModel> m_vxlHotbarModel{ nullptr };
		std::unique_ptr<vxlModel> m_vxlCrosshairModel{ nullptr };

		// The voxel world
		std::unique_ptr<vxlWorld> m_vxlWorld{};
	};
}
