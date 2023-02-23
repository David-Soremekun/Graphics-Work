#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"



class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };
	GLuint m_program2{ 0 };
	// Vertex Array Object to wrap all render settings
	std::vector<GLuint>  m_VAO;
	float propRot{ 0.f };
	GLuint texture;
	// Number of elments to use when rendering
	std::vector<GLuint>m_numElements;

	bool m_wireframe{ false };
	bool m_cullFace{ true };
	

public:
	Renderer();
	~Renderer();
	glm::mat4 translation(glm::mat4 modelMatrix, glm::vec3 translation);
	glm::mat4 rotation(glm::mat4 modelMatrix,GLuint angle, glm::vec3 axis);

	// Draw GUI
	void DefineGUI();

	// Create the program. This is the compiled shaders.
	bool CreateProgram(std::string ShaderName, std::string FragName, GLuint& mprogram);

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();
	int loadCubeMap();
	bool CreateTerrain(int numberOfCellsX, int numberOfCellsZ,float sizeX, float sizeZ, const std::string&textureFilename);
	float GenerateNoise(int x, int y);


	GLuint& getProgram() { return m_program; }
	GLuint& getProgram2() { return m_program2; }

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

