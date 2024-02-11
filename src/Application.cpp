#include "Classes/Public/Renderer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <map>
#include <algorithm>

#include "Classes/Public/Model.h"
#include "Classes/Public/Shader.h"
#include "Classes/Public/Camera.h"
#include "Classes/Public/Mesh.h"
#include "Classes/Public/ChessBoard.h"
#include "enums/ObjectType.h"
#include "Classes/Public/Bezier.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

static std::shared_ptr<ChessBoard> Setup()
{
	std::map<int, std::shared_ptr<Mesh>> meshes = {
		{ OT_Pawn, std::shared_ptr<Mesh>(new Mesh("res/textures/pawn/pawn.obj")) },
		{ OT_Rook, std::shared_ptr<Mesh>(new Mesh("res/textures/rook/rook.obj")) },
		{ OT_Knight, std::shared_ptr<Mesh>(new Mesh("res/textures/knight/knight.obj")) },
		{ OT_Bishop, std::shared_ptr<Mesh>(new Mesh("res/textures/bishop/bishop.obj")) },
		{ OT_Queen, std::shared_ptr<Mesh>(new Mesh("res/textures/queen/queen.obj")) },
		{ OT_King, std::shared_ptr<Mesh>(new Mesh("res/textures/king/king.obj")) },
		{ OT_Board, std::shared_ptr<Mesh>(new Mesh("res/textures/board/board.obj")) }
	};
	std::shared_ptr<Texture> chessPieceTexture ( new Texture("res/textures/pieceTex.JPG"));
	std::shared_ptr<Texture> chessboardTexture (new Texture("res/textures/board/chessboard.jpg"));

	std::shared_ptr<Mesh> bezierMesh((Mesh*)new Bezier());
	float bezierScale = 8.0f;

	Model::meshMap = meshes;

	// without bezier
	//std::shared_ptr<ChessBoard> Board(new ChessBoard(meshes[OT_Board], chessboardTexture, glm::vec3(0.f, -2.f, 0.f)));
	// with bezier
	std::shared_ptr<ChessBoard> Board (new ChessBoard(bezierMesh, chessboardTexture, glm::vec3(-4.f, -1.8f, 4.f)));
	Board->SetScale(bezierScale);
	Board->RotateY(90.0f);
	//

	float scale = 0.15f;
	std::map<int, std::shared_ptr<Model>> chessPiecesModels;
	for (int i = OT_Pawn; i <= OT_King; i++)
	{
		chessPiecesModels[i] = std::shared_ptr<Model>(new Model(Model::meshMap[i], chessPieceTexture));
		chessPiecesModels[i]->SetScale(scale);
	}

	ChessBoard::piecesModelsMap = chessPiecesModels;
	
	Board->RotateY(glm::radians(-90.f));

	//white pieces
	Board->AddPiece(OT_Rook, false, 0, 0);
	Board->AddPiece(OT_Queen, false, 3, 3);
	Board->AddPiece(OT_Knight, false, 4, 2);
	Board->AddPiece(OT_Bishop, false, 2, 5);
	Board->AddPiece(OT_King, false, 3, 0);
	Board->AddPiece(OT_Pawn, false, 0, 3);
	Board->AddPiece(OT_Pawn, false, 7, 2);
	Board->AddPiece(OT_Pawn, true, 3, 4);
	Board->AddPiece(OT_King, true, 3, 7);
	Board->AddPiece(OT_Pawn, true, 7, 5);
	Board->AddPiece(OT_Bishop, true, 0, 6);
	Board->AddPiece(OT_Queen, true, 5, 5);
	Board->AddPiece(OT_Bishop, true, 6, 7);
	Board->AddPiece(OT_Rook, true, 2, 2);
	return Board;
}

static void SwitchCamerasInput(std::vector<std::shared_ptr<Camera>>& cameras, GLFWwindow* window)
{
	// first camera
	if (glfwGetKey(window, GLFW_KEY_1))
	{
		Camera::m_CurrCam = cameras[0];
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_2))
	{
		Camera::m_CurrCam = cameras[1];
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_3))
	{
		Camera::m_CurrCam = cameras[2];
		return;
	}
}

static glm::vec3 RotateVertically(glm::vec3 v, float angle)
{
	glm::vec3 axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), v);
	axis = glm::normalize(axis);
	return glm::rotate(v, angle, axis);
}

static void OtherInput(bool& fog, GLFWwindow* window, glm::vec3& SpotLightDir)
{
	float rotationSpeed = 2.f;
	// Fog
	if (glfwGetKey(window, GLFW_KEY_F))
	{
		fog = !fog;
		return;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		SpotLightDir = glm::rotateY(SpotLightDir, glm::radians(rotationSpeed));
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		SpotLightDir = glm::rotateY(SpotLightDir, glm::radians(-rotationSpeed));
	}
	else if (glfwGetKey(window, GLFW_KEY_UP))
	{
		SpotLightDir = RotateVertically(SpotLightDir, glm::radians(-rotationSpeed));
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		SpotLightDir = RotateVertically(SpotLightDir, glm::radians(rotationSpeed));
	}
}

int main()
{
	// Initialize GLFW
	GLFWwindow* window;

	if(!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess 3D", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		return -2;

	std::cout << glGetString(GL_VERSION) << "\n";

	glEnable(GL_DEPTH_TEST);

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA));
	glEnable(GL_BLEND);

	std::shared_ptr<ChessBoard> Board = Setup();

	std::vector<std::shared_ptr<Shader>> Shaders;
	std::shared_ptr<Shader> PhongShader(new Shader("res/shaders/Phong.shader"));

	Shader::m_CurrShader = PhongShader;

	Renderer renderer;
	
#pragma region Cameras
	std::vector<std::shared_ptr<Camera>> Cameras;
	std::shared_ptr<Camera> Freecamera (new Camera(glm::vec3(2.f, 12.f, 25.f)));
	std::shared_ptr<Camera> FocusedCamera(new Camera(glm::vec3(0.0f, 1.5f, 0.0f)));
	FocusedCamera->SetFOVdeg(1.2f);
	std::shared_ptr<Camera> FPCamera(new Camera());
	FPCamera->SetFOVdeg(1.2f);
	FocusedCamera->BlockInput = true;
	FPCamera->BlockInput = true;

	glfwSetScrollCallback(window, Camera::Scroll_callback);

	Cameras.push_back(Freecamera);
	Cameras.push_back(FocusedCamera);
	Cameras.push_back(FPCamera);
	Camera::m_CurrCam = Freecamera;
#pragma endregion

	std::shared_ptr<Shader> lightShader (new Shader("res/shaders/Light.shader"));
	std::shared_ptr<Mesh> LightMesh (new Mesh("res/textures/light/lightbulb.obj"));
	std::shared_ptr<Model> LightBulb (new Model(LightMesh, nullptr, glm::vec3(-2.f, 2.f, 0.f)));
	std::shared_ptr<Model> LightBulb2 (new Model(LightMesh, nullptr, glm::vec3(2.f, 2.f, 0.f)));
	LightBulb->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	LightBulb2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	FPCamera->RotateHorizontally(glm::radians(90.0f));

	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;

	static clock::time_point start = clock::now();
	duration elapsed = clock::now() - start;

	bool Fog = false;

#pragma region Moving knight
	std::shared_ptr<Model> MovingKnight(new Model(Model::meshMap[OT_Knight], std::shared_ptr<Texture>(new Texture("res/textures/pieceTex.JPG"))));
	MovingKnight->SetScale(glm::vec3(0.15f, 0.15f, 0.15f));
	glm::vec3 cb_A1Pos(-3.5f, -1.9f, 3.5f);
	float minRow = 1.f;
	float maxRow = 6.f;
	float minCol = 1.f;
	float maxCol = 6.f;
	glm::vec3 curPos(minRow, 0.0f, minCol);
	MovingKnight->SetPosition(glm::vec3(cb_A1Pos.x + curPos.x, cb_A1Pos.y, cb_A1Pos.z - curPos.z));
	MovingKnight->RotateY(-90.0f);
	bool beginTurn = false;
	float finalAngle = MovingKnight->GetRotation().y;

	glm::vec3 speed(0.0f, 0.0f, -0.08f);
	speed = glm::cross(speed, glm::vec3(0.0f, 1.0f, 0.0f));
	float angleSpeed = 10.0f;
#pragma endregion
	glm::vec3 GreenSpotLightDir = FPCamera->GetOrientation();
	glm::vec3 RedSpotLightDir = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), GreenSpotLightDir);
	RedSpotLightDir.y -= 0.5f;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		renderer.Clear();

		SwitchCamerasInput(Cameras, window);
		OtherInput(Fog, window, RedSpotLightDir);

		Shader::m_CurrShader->Bind();

		Camera::m_CurrCam->Inputs(window, *Shader::m_CurrShader);
		Camera::m_CurrCam->UpdateUniform(*Shader::m_CurrShader);

		Shader::m_CurrShader->SetUniform1i("u_FogEnabled", Fog);

		Shader::m_CurrShader->SetUniform4f("lights[0].m_LightColor", 1.f, 1.f, 1.f, 1.f);
		Shader::m_CurrShader->SetUniform3f("lights[0].m_LightPos", LightBulb->GetPosition());
		Shader::m_CurrShader->SetUniform1i("lights[0].m_IsPointLight", true);

		Shader::m_CurrShader->SetUniform4f("lights[1].m_LightColor", 1.f, 1.f, 1.f, 1.f);
		Shader::m_CurrShader->SetUniform3f("lights[1].m_LightPos", LightBulb2->GetPosition());
		Shader::m_CurrShader->SetUniform1i("lights[1].m_IsPointLight", true);

		// SpotLights
		glm::vec3 GreenSpotLightPos(MovingKnight->GetPosition());
		glm::vec3 RedSpotLightPos(MovingKnight->GetPosition());
		glm::vec3 GreenSpotLightDir = FPCamera->GetOrientation();
		GreenSpotLightDir.y -= 0.5f;
		GreenSpotLightPos.y += 1.5f;
		RedSpotLightPos.y += 1.5f;

		Shader::m_CurrShader->SetUniform4f("lights[2].m_LightColor", 0.f, 1.f, 0.f, 1.f);
		Shader::m_CurrShader->SetUniform3f("lights[2].m_LightPos", GreenSpotLightPos);
		Shader::m_CurrShader->SetUniform1i("lights[2].m_IsPointLight", false);
		Shader::m_CurrShader->SetUniform3f("lights[2].m_LightDir", GreenSpotLightDir);

		Shader::m_CurrShader->SetUniform4f("lights[3].m_LightColor", 1.f, 0.f, 0.f, 1.f);
		Shader::m_CurrShader->SetUniform3f("lights[3].m_LightPos", RedSpotLightPos);
		Shader::m_CurrShader->SetUniform1i("lights[3].m_IsPointLight", false);
		Shader::m_CurrShader->SetUniform3f("lights[3].m_LightDir", RedSpotLightDir);

		Shader::m_CurrShader->SetUniform3f("u_ViewPos", Freecamera->GetPosition());
		Board->Draw(renderer, *Shader::m_CurrShader);
		//Shader::m_CurrShader->SetUniform4f("u_Color", 0.4f, 0.4f, 0.4f, 1.f);

		// moving knight
		duration elapsed = clock::now() - start;
		curPos += speed;
		// movement control
		if (curPos.x > maxRow || curPos.x < minRow || curPos.z > maxRow || curPos.z < minRow)
		{
			speed = glm::cross(speed, glm::vec3(0.0f, 1.0f, 0.0f));
			curPos.x = std::min(curPos.x, maxRow);
			curPos.x = std::max(curPos.x, minRow);
			curPos.z = std::min(curPos.z, maxCol);
			curPos.z = std::max(curPos.z, minCol);
			beginTurn = true;
			finalAngle = MovingKnight->GetRotation().y + 90.0f;
		}
		// knight and FPCamera rotation control
		if (beginTurn)
		{
			MovingKnight->RotateY(angleSpeed);
			FPCamera->RotateHorizontally(glm::radians(-angleSpeed));
			RedSpotLightDir = glm::rotateY(RedSpotLightDir, glm::radians(angleSpeed));
			if (MovingKnight->GetRotation().y >= finalAngle)
			{
				beginTurn = false;
				if (MovingKnight->GetRotation().y >= 360.0f)
					MovingKnight->SetRotationY(0.0f); // in case of overflow
			}
		}
		float newY = 0;
		MovingKnight->SetPosition(glm::vec3(cb_A1Pos.x + curPos.x, cb_A1Pos.y + newY, cb_A1Pos.z - curPos.z));
		MovingKnight->Draw(*Shader::m_CurrShader);

		FocusedCamera->LookAt(MovingKnight->GetPosition());
		glm::vec3 FPCamPos = MovingKnight->GetPosition();
		FPCamPos.y += 1.0f;
		FPCamera->SetPosition(FPCamPos);

		// bezier animation
		Board->Tick(elapsed.count() / 100);

		// lights
		lightShader->Bind();
		Camera::m_CurrCam->UpdateUniform(*lightShader);
		lightShader->SetUniform4f("u_Color", 1.f, 1.f, 1.f, 1.f);
		LightBulb->Draw(*lightShader);
		LightBulb2->Draw(*lightShader);
		

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		
		// Take care of all GLFW events
		glfwPollEvents();

		start = clock::now();
	}

	// Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
