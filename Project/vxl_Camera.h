#pragma once
#include <queue>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


namespace vxl
{
	class WorldObserver;
}

namespace vxl
{
	class vxlWindow;
	class vxlCamera
	{
	public:
		explicit vxlCamera(vxlWindow& window);
		~vxlCamera();

		vxlCamera(const vxlCamera& other) = delete;
		vxlCamera(vxlCamera&& other) = delete;
		vxlCamera& operator=(const vxlCamera& other) = delete;
		vxlCamera& operator=(vxlCamera&& other) = delete;

		void Update(float deltaTime);

		// Adding the world as an observer to be able to pass through the fact that there is a mouse click & camera data
		void AddObserver(WorldObserver* observer) { m_worldObserver = observer; }

		// Callbacks
		void OnMouseMoved(double xposIn, double yposIn);				// FPS camera look-around
		void OnScroll(double xoffset, double yoffset);					// Updates FOV on scroll
		void OnKeyPress(int key, int scancode, int action, int mods);	// Spectator mode fly movement & changing selected block
		void OnMouseButton(int key, int action, int mods);				// Performs raycast at the current camera position & forward direction

		// Getters
		glm::mat4 GetViewMatrix() const { return lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp); }
		glm::vec3 GetPosition() const { return m_cameraPos; }
		glm::vec3 GetForward() const { return m_cameraFront; }
		float GetFOV() const { return m_fov; }


	private:
		// Interface that implements OnMouseButton and OnKeyPress
		// Allows us to keep the Callback functions that GLFW will use in one location
		// while also allowing us to call the necessary functions in the vxlWorld class
		WorldObserver* m_worldObserver;
		vxlWindow& m_window;

		void Init(); // Sets the callback functions in the window class

		void ProcessInput(float deltaTime);
		std::unordered_map<int, bool> m_keyState;

		// Camera data - See OpenGL tutorial: https://learnopengl.com/Getting-started/Camera
		glm::vec3 m_cameraPos{};
		glm::vec3 m_cameraFront{};
		glm::vec3 m_cameraUp{};

		bool m_firstMouse{};
		float m_yaw{};
		float m_pitch{};
		float m_lastX{};
		float m_lastY{};
		float m_fov{};
		float m_speed{ 10 };
	};
}

