#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H

#include "glm/glm.hpp"

namespace Navigator {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		inline void SetPosition(const glm::vec3& coords) { m_position = coords; RecalculateViewMatrix(); }
		inline const glm::vec3& GetPosition() const { return m_position; }
		inline void SetPolarRotation(float degrees) { m_polarRot = degrees; RecalculateViewMatrix(); }
		inline float GetRotation() const { return m_polarRot; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();

		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position;
		float m_polarRot;
	};

}

#endif