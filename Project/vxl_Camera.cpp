#include "vxl_Camera.h"
#include "vxl_app.h"


vxl::vxlCamera::vxlCamera(vxlWindow& window) :
	m_window(window),
	m_cameraPos{ glm::vec3{0.0f,104.0f,0.0f} },
	m_cameraFront{ glm::vec3(0.0f, 0.0f, -1.0f) },
	m_cameraUp{ glm::vec3(0.0f, 1.0f, 0.0f) },
	m_firstMouse{ true },
	m_yaw{ -90.f },
	m_pitch{ 0.0f },
	m_lastX{ vxlApp::WIDTH / 2.0f },
	m_lastY{ vxlApp::HEIGHT / 2.0f },
	m_fov{ 45.0f }
{
	Init();
}

vxl::vxlCamera::~vxlCamera()
= default;

void vxl::vxlCamera::Init()
{
	m_window.SetMouseMovementCallback([this](GLFWwindow* window, double xpos, double ypos) {
		OnMouseMoved(xpos, ypos);
		});
	m_window.setKeyCallback([this](GLFWwindow* window, int key, int scancode, int action, int mods) {
		OnKeyPress(key, scancode, action, mods);
		});
	m_window.setScrollCallback([this](GLFWwindow* window, double xoffset, double yoffset) {
		OnScroll(xoffset, yoffset);
		});
	m_window.SetMouseButtonCallback([this](GLFWwindow* window, int key, int action, int mods)
		{
			OnMouseButton(key, action, mods);
		});
}

void vxl::vxlCamera::Update(float deltaTime)
{
	ProcessInput(deltaTime);

}

void vxl::vxlCamera::OnMouseMoved(double xposIn, double yposIn)
{
	const float xpos = static_cast<float>(xposIn);
	const float ypos = static_cast<float>(yposIn);

	if (m_firstMouse)
	{
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstMouse = false;
	}

	float xoffset = xpos - m_lastX;
	float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top
	m_lastX = xpos;
	m_lastY = ypos;

	const float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_cameraFront = glm::normalize(front);
}

void vxl::vxlCamera::OnScroll(double xoffset, double yoffset)
{
	m_fov -= (float)yoffset;
	if (m_fov < 1.0f)
		m_fov = 1.0f;
	if (m_fov > 45.0f)
		m_fov = 45.0f;
}

void vxl::vxlCamera::OnKeyPress(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		keyState[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		keyState[key] = false;
	}
}

std::unordered_map<int, bool> mouseButtonState;

void vxl::vxlCamera::OnMouseButton(int key, int action, int mods)
{
	m_worldObserver->OnMouseButton(key, action, mods);
}

void vxl::vxlCamera::ProcessInput(float deltaTime)
{
	if (keyState[GLFW_KEY_ESCAPE]) {
		glfwSetWindowShouldClose(m_window.GetGLFWWindow(), true);
	}

	const float cameraSpeed = m_speed * deltaTime;
	if (keyState[GLFW_KEY_W]) 
	{
		m_cameraPos += cameraSpeed * m_cameraFront;
	}
	if (keyState[GLFW_KEY_S]) 
	{
		m_cameraPos -= cameraSpeed * m_cameraFront;
	}
	if (keyState[GLFW_KEY_A]) 
	{
		m_cameraPos -= normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
	}
	if (keyState[GLFW_KEY_D]) 
	{
		m_cameraPos += normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
	}
	if (keyState[GLFW_KEY_SPACE])
	{
		m_cameraPos += glm::vec3{ 0, 1, 0 } *cameraSpeed;
	}
	if (keyState[GLFW_KEY_LEFT_SHIFT]) 
	{
		m_cameraPos -= glm::vec3{ 0, 1, 0 } *cameraSpeed;
	}
	if(keyState[GLFW_KEY_LEFT_CONTROL])
	{
		m_speed = 25.0f;
	}else
	{
		m_speed = 10.0f;
	}

	
}
