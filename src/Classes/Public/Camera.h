#pragma once
#include "Shader.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "memory"

class Camera
{
private:
	// Stores the main vectors of the camera
	glm::vec3 m_Position;
	glm::vec3 m_Orientation = glm::vec3(0.0f, -0.2f, -1.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f); // Global Up
	glm::mat4 m_CameraMatrix = glm::mat4(1.0f);

	int m_WindowWidth;
	int m_WindowHeight;
	float m_NearPlane;
	float m_FarPlane;
	float m_FOVdeg;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Adjust the speed of the camera and it's sensitivity when looking around
	static float m_Speed;
	static float m_Sensitivity;

public:
	static std::shared_ptr<Camera> m_CurrCam;
	// called when scroll is triggered
	static void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	bool BlockInput = false;

	// Camera constructor to set up initial values
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f), int windowWidth = 800, int windowHeight = 800,
		float nearPlane = 0.8f, float farPlane = 100.0f, float FOVdeg = 0.7f);

	// Exports the camera matrix to a shader
	void UpdateUniform(Shader& shader);
	void LookAt(glm::vec3 lookAtPoint);

	void Move(glm::vec3 v);
	void MoveForwardsBackwards(float dist);
	void MoveSideways(float dist);
	void MoveUpDown(float dist);

	void RotateVertically(float deg);
	void RotateHorizontally(float deg);


	// Handles camera inputs
	void Inputs(GLFWwindow* window, Shader& shader);

	void SetFOVdeg(float FOVdeg) { m_FOVdeg = FOVdeg; };
	float GetSpeed() { return m_Speed; }
	float GetSensitivity() { return m_Sensitivity; }

	void SetPosition(glm::vec3 nPos) { m_Position = nPos; };
	glm::vec3 GetPosition() const { return m_Position; }

	glm::vec3 GetOrientation() const { return m_Orientation; }

	// sets scroll reaction
	static void SetScrollInput(GLFWwindow* window, bool value = true);

private:
	void UpdateMatrix();
};