#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

struct AquaPig
{
	
	
	
};

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

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
	bool CreateProgram();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

