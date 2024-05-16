#include "Vxl_Window.h"

#include <stdexcept>
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
		glfwSetWindowUserPointer(m_window, this);
		//Set the function that glfw should call when it realizes m_window has been resized
		glfwSetWindowSizeCallback(m_window, FrameBufferResizeCallback);
	}

	void vxlWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto w = reinterpret_cast<vxlWindow*>(glfwGetWindowUserPointer(window));
		w->m_frameBufferResized = true;
		w->m_width = width;
		w->m_height = height;
	}
}