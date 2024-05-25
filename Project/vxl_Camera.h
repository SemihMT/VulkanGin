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

	struct CameraState
	{
		int button;
		glm::vec3 cameraPos{};
		glm::vec3 cameraFront{};
		glm::vec3 cameraUp{};
	};

	class vxlCamera
	{


	public:
		explicit vxlCamera(vxlWindow& window);
		~vxlCamera();


		vxlCamera(const vxlCamera& other) = delete;
		vxlCamera(vxlCamera&& other) = delete;
		vxlCamera& operator=(const vxlCamera& other) = delete;
		vxlCamera& operator=(vxlCamera&& other) = delete;

		void AddObserver(WorldObserver* observer) {	m_worldObserver = observer; }

		void OnMouseMoved(double xposIn, double yposIn);
		void OnScroll(double xoffset, double yoffset);
		void OnKeyPress(int key, int scancode, int action, int mods);
		void OnMouseButton(int key, int action, int mods);

		void Update(float deltaTime);
		glm::mat4 GetViewMatrix() const { return lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp); }
		glm::vec3 GetPosition() const { return m_cameraPos; }
		glm::vec3 GetForward() const { return m_cameraFront; }
		float GetFOV() const { return m_fov; }


	private:
		WorldObserver* m_worldObserver;

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
		float m_speed{ 10 };
	};
}

