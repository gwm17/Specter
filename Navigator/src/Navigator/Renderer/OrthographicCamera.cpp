#include "OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Navigator {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) :
		m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_viewMatrix(1.0f), m_viewProjectionMatrix(1.0f), m_position(0.0f, 0.0f, 0.0f), m_polarRot(0.0f)
	{
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix; //OpenGL orderer is proj*view, dx is view*proj
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform_matrix = glm::translate(glm::mat4(1.0f), m_position); //translate
		transform_matrix = glm::rotate(transform_matrix, glm::radians(m_polarRot), glm::vec3(0.0f, 0.0f, 1.0f)); //then rotate

		m_viewMatrix = glm::inverse(transform_matrix); //view is the inverse of the transform to the camera

		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

}