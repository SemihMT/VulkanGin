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
		// This struct holds functions that the GLFW callback functions will call
		// The actual functions the GLFW framework calls are defined below.
		// The reason for this struct existing is to be able to assign these functions from anywhere with access to the window
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

		bool ShouldClose();														// Wraps 'glfwWindowShouldClose'
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);	// Tries to create a window surface we can render on
		VkExtent2D GetExtent() const;											// Returns the width and height of the window

		GLFWwindow* GetGLFWWindow() const { return m_window; }
		bool WasWindowResized() const { return m_frameBufferResized; }
		void ResetWindowResizedFlag() { m_frameBufferResized = false; }

		// Setters for the std::function objects in 'm_callbacks'
		void SetMouseMovementCallback(std::function<void(GLFWwindow*, double, double)> callback);
		void setScrollCallback(std::function<void(GLFWwindow*, double, double)> callback);
		void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback);
		void SetMouseButtonCallback(std::function<void(GLFWwindow*, int, int, int)>);

	private:
		void InitWindow();

		// GLFW Callback functions
		// These check if their respective function is set in 'm_callbacks' and call them if they are 
		static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
		static void MouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		int m_width{};
		int m_height{};
		bool m_frameBufferResized{ false };
		std::string m_windowName{};
		GLFWwindow* m_window{};
		vxlWindowCallbacks m_callbacks;

	};
}
