#pragma once
#define GLFW_INCLUDE_VULKAN	
#include "GLFW/glfw3.h"
#include <string>

namespace vxl
{
	class vxlWindow
	{
	public:
		vxlWindow(int w, int h, const std::string& name);
		~vxlWindow();
		


		vxlWindow(const vxlWindow& other) = delete;
		vxlWindow(vxlWindow&& other) = delete;
		vxlWindow& operator=(const vxlWindow& other) = delete;
		vxlWindow& operator=(vxlWindow&& other) = delete;

		bool WasWindowResized() { return m_frameBufferResized; }
		void ResetWindowResizedFlag() { m_frameBufferResized = false; }
		bool ShouldClose();
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }

	private:
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void InitWindow();


		int m_width{};
		int m_height{};
		bool m_frameBufferResized{ false };
		std::string m_windowName{};
		GLFWwindow* m_window{};

	};
}
