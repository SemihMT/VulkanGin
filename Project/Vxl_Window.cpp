#include "Vxl_Window.h"

#include <stdexcept>

#include "vxl_Camera.h"

namespace vxl {
	vxlWindow::vxlWindow(int w, int h, const std::string& name) :
		m_width(w),
		m_height(h),
		m_windowName(name)
	{
		InitWindow();
	}

	vxlWindow::~vxlWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool vxlWindow::ShouldClose()
	{
		return glfwWindowShouldClose(m_window);
	}

	void vxlWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create a window surface!");
		}
	}

	void vxlWindow::InitWindow()
	{
		//init lib
		glfwInit();
		//Tell glfw we're not using opengl
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//Make window resizable
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
		//Pair the glfw window object with this window class (just stores a ptr with the window)
		glfwSetWindowUserPointer(m_window, &m_callbacks);
		//Set the function that glfw should call when it realizes m_window has been resized
		glfwSetWindowSizeCallback(m_window, FrameBufferResizeCallback);

		glfwSetCursorPosCallback(m_window, MouseMovementCallback);
		glfwSetKeyCallback(m_window, KeyCallback);
		glfwSetScrollCallback(m_window, ScrollCallback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	}


	void vxlWindow::SetMouseMovementCallback(std::function<void(GLFWwindow*, double, double)> callback)
	{
		m_callbacks.mouseMoveCallback = callback;

	}

	void vxlWindow::setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> callback)
	{
		m_callbacks.keyCallback = callback;
	}

	void vxlWindow::setScrollCallback(std::function<void(GLFWwindow*, double, double)> callback)
	{
		m_callbacks.scrollCallback = callback;
	}

	void vxlWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		const auto w = static_cast<vxlWindow*>(glfwGetWindowUserPointer(window));
		w->m_frameBufferResized = true;
		w->m_width = width;
		w->m_height = height;
	}

	void vxlWindow::MouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
	{
		const vxlWindowCallbacks* callbacks = static_cast<vxlWindowCallbacks*>(glfwGetWindowUserPointer(window));
		if (callbacks && callbacks->mouseMoveCallback) {
			callbacks->mouseMoveCallback(window, xpos, ypos);
		}
	}

	void vxlWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		const vxlWindowCallbacks* callbacks = static_cast<vxlWindowCallbacks*>(glfwGetWindowUserPointer(window));
		if (callbacks && callbacks->scrollCallback) {
			callbacks->scrollCallback(window, xoffset, yoffset);
		}
	}

	void vxlWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		const vxlWindowCallbacks* callbacks = static_cast<vxlWindowCallbacks*>(glfwGetWindowUserPointer(window));
		if (callbacks && callbacks->keyCallback) {
			callbacks->keyCallback(window, key, scancode, action, mods);
		}
	}
}
