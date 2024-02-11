#include "../Public/Camera.h"

std::shared_ptr<Camera> Camera::m_CurrCam = nullptr;
float Camera::m_Speed = 0.2f;
float Camera::m_Sensitivity = 5.0f;

Camera::Camera(glm::vec3 position, glm::vec3 lookAtPoint, int windowWidth, int windowHeight,
	float nearPlane, float farPlane, float FOVdeg)
	: m_Position(position), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight),
	m_NearPlane(nearPlane), m_FarPlane(farPlane), m_FOVdeg(FOVdeg)
{
	try
	{
		if (position == lookAtPoint)
			throw 501;
		m_Orientation = lookAtPoint - position;
		m_Orientation = glm::normalize(m_Orientation);
	}
	catch (int ex)
	{
		std::cout << "Error: " << ex << std::endl;
		std::cout << "Position is the same as lookAt point!";
	}
}

void Camera::UpdateUniform(Shader& shader)
{
	UpdateMatrix();
	shader.SetUniformMatrix4fv("u_camMatrix", glm::value_ptr(m_CameraMatrix));
}

void Camera::LookAt(glm::vec3 lookAtPoint)
{
	m_Orientation = lookAtPoint - m_Position;
}

void Camera::Move(glm::vec3 v)
{

}

// Moves camera forwards if dist is positive, backwards if negative
void Camera::MoveForwardsBackwards(float dist)
{
	m_Position += m_Orientation * dist;
}

// Right if dist > 0
void Camera::MoveSideways(float dist)
{
	glm::vec3 dir = glm::normalize(glm::cross(m_Orientation, m_Up));

	m_Position += dir * dist;
}

// Up if dist > 0
void Camera::MoveUpDown(float dist)
{
	glm::vec3 right = glm::cross(m_Orientation, m_Up);
	glm::vec3 dir = glm::normalize(glm::cross(right, m_Orientation));

	m_Position += dir * dist;
}

// Rotates camera around ortogonal to Orientation and Up, up turn if deg > 0
// degree in radians 
// Camera cannot be turned upside down
void Camera::RotateVertically(float deg)
{
	float angle = glm::angle(m_Up, m_Orientation);
	if (deg > 0)
		angle = angle;
	if ((angle < 0.1f && deg < 0) || (angle > 2.9 && deg > 0))
		return;
	glm::vec3 axis = glm::cross(m_Up, m_Orientation);

	m_Orientation = glm::rotate(m_Orientation, deg, axis);
}

// Rotates camera around gloal Up, right turn if deg > 0
// degree in radians 
// Camera cannot be turned upside down
void Camera::RotateHorizontally(float deg)
{
	m_Orientation = glm::rotate(m_Orientation, -deg, m_Up);
}

void Camera::Inputs(GLFWwindow* window, Shader& shader)
{
	// Handles key inputs

	if (BlockInput)
		return;

	// Up Down
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		MoveUpDown(m_Speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		MoveUpDown(-m_Speed);
	}

	// Right Left
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		MoveSideways(m_Speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		MoveSideways(-m_Speed);
	}


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (m_WindowWidth / 2), (m_WindowHeight / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = m_Sensitivity * (float)(mouseY - (m_WindowHeight / 2)) / m_WindowWidth;
		float rotY = m_Sensitivity * (float)(mouseX - (m_WindowWidth / 2)) / m_WindowHeight;

		// Calculates upcoming vertical change in the Orientation
		RotateVertically(rotX);

		// Rotates the Orientation left and right
		RotateHorizontally(rotY);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (m_WindowWidth / 2), (m_WindowHeight / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

	UpdateUniform(shader);
}

void Camera::UpdateMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
	projection = glm::perspective(m_FOVdeg, (float)m_WindowWidth / m_WindowHeight, m_NearPlane, m_FarPlane);

	m_CameraMatrix = projection * view;
}

void Camera::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!m_CurrCam->BlockInput)
		Camera::m_CurrCam->MoveForwardsBackwards(yoffset * m_Speed * 2);
}
