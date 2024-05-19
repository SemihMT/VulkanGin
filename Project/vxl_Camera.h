#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>


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

		void OnMouseMoved(double xposIn, double yposIn);
		void OnScroll(double xoffset, double yoffset);
		void OnKeyPress(int key, int scancode, int action, int mods);
		void Update(float deltaTime);
		glm::mat4 GetViewMatrix() const { return lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp); }
		float GetFOV() const { return m_fov; }


	private:
		void Init();
		void ProcessInput(float deltaTime);


		vxlWindow& m_window;
		std::unordered_map<int, bool> keyState;
		// camera
		glm::vec3 m_cameraPos{};
		glm::vec3 m_cameraFront{};
		glm::vec3 m_cameraUp{};

		bool m_firstMouse{};
		float m_yaw{};	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		float m_pitch{};
		float m_lastX{};
		float m_lastY{};
		float m_fov{};
	};
}

