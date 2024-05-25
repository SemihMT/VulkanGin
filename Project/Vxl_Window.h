#pragma once
#define GLFW_INCLUDE_VULKAN	
#include <functional>

#include "GLFW/glfw3.h"
#include <string>

namespace vxl
{
	class vxlWindow
	{
	public:
		struct vxlWindowCallbacks
		{
			std::function<void(GLFWwindow*, double, double)> mouseMoveCallback;
			std::function<void(GLFWwindow*, int, int, int, int)> keyCallback;
			std::function<void(GLFWwindow*, double, double)> scrollCallback;
			std::function<void(GLFWwindow*, int, int, int)> mouseButtonCallback;
			
		};
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

		GLFWwindow* GetGLFWWindow() const { return m_window; }

		void SetMouseMovementCallback(std::function<void(GLFWwindow*, double, double)> callback);
		void setScrollCallback(std::function<void(GLFWwindow*, double, double)> callback);
		void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback);
		void SetMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)>);
	private:
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
		static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		void InitWindow();
		int m_width{};
		int m_height{};
		bool m_frameBufferResized{ false };
		std::string m_windowName{};
		GLFWwindow* m_window{};
		vxlWindowCallbacks m_callbacks;

	};
}
