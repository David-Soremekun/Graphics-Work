#include "Renderer.h"
#include "Camera.h"
#include <cmath>
Renderer::Renderer()
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_VAO);
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return true;
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	/*
		TODO 1: The first step is to create the vertices, colours and indices
		You could use std::vectors to hold these*/
	std::vector<glm::vec3> verts =
	{

		glm::vec3(10.0f, 10.0f, -10.0f),
		glm::vec3(-10.0f, 10.0f, -10.0f),
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),

		// Bottom face (y = -1.0f)
		 // Orange
		glm::vec3(10.0f, -10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(10.0f, -10.0f, -10.0f),

		// Front face  (z = 1.0f)
			 // Red
		glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),

		// Back face (z = -1.0f)
		   // Yellow
		glm::vec3(10.0f, -10.0f, -10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(-10.0f, 10.0f, -10.0f),
		glm::vec3(10.0f, 10.0f, -10.0f),

		// Left face (x = -1.0f)
			// Blue
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, 10.0f, -10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),

		// Right face (x = 1.0f)
			// Magenta
		glm::vec3(10.0f, 10.0f, -10.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, -10.0f),




	};
	/*The cube is 20 by 20 centred on 0, 0, 0
	so the minimum values for x, y, z are -10,-10,-10
	and the maximum values are 10,10,10
	*/
	std::vector<glm::vec3> vertColour = {
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),

		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),

		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),

		glm::vec3(1,0,1),
		glm::vec3(1,0,1),
		glm::vec3(1,0,1),
		glm::vec3(1,0,1),

		glm::vec3(1,1,0),
		glm::vec3(1,1,0),
		glm::vec3(1,1,0),
		glm::vec3(1,1,0),

		glm::vec3(0,1,1),
		glm::vec3(0,1,1),
		glm::vec3(0,1,1),
		glm::vec3(0,1,1)


	};

	std::vector<GLuint> elements = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};



	/*The cube is 20 by 20 centred on 0, 0, 0
	so the minimum values for x, y, z are -10,-10,-10
	and the maximum values are 10,10,10
	*/


	/*
		TODO 2: Next you need to create VBOs for the vertices and the colours
		You can look back to last week for examples
	*/

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	GLuint ColourVB0;
	glGenBuffers(1, &ColourVB0);
	glBindBuffer(GL_ARRAY_BUFFER, ColourVB0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertColour.size(), vertColour.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	/*
		TODO 3: You also need to create an element buffer
		Store the number of elements in the member variable m_numElements
	*/

	GLuint ElementVBO;
	m_numElements = elements.size();
	glGenBuffers(1, &ElementVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_numElements, elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*
		TODO 4: Finally create a VAO to wrap the buffers. You need to specify the streams for the positions
		(attribute 0) and colours (attribute 1). You also need to bind the element buffer.
		Use the member variable m_VAO
	*/
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer
	(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindBuffer(GL_ARRAY_BUFFER, ColourVB0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer
	(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementVBO);
	glBindVertexArray(0);
	/*
		TODO 5: Run it and see if you can see the cube.
		You should not have to edit the render function or shaders to see the cube.
	*/

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 400.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	// Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	glm::mat4 model_xform = glm::mat4(1);

	// Uncomment all the lines below to rotate cube first round y then round x
	static float angle = 0;
	static float degrees = 0;
	
	float translation = 2 * sinf(degrees * 3.141 / 180);
	
	static bool rotateY = true;
	static bool translateY = true;

	if (rotateY && translateY) { // Rotate around y axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 0 ,1,0 });
		model_xform = glm::translate(model_xform, glm::vec3{ translation,0, 1.0 });
	}
	else // Rotate around x axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 1 ,0,0 });
		model_xform = glm::translate(model_xform, glm::vec3{  0,translation, 1.0 });

	angle += 0.001f;
	degrees += 0.001f;
	if (angle > glm::two_pi<float>() || degrees >= 360.f)
	{
		angle = 0;
		degrees = 0;
		rotateY = !rotateY;
		translateY = !translateY;
	}


	// Send the model matrix to the shader in a uniform
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	// Bind our VAO and render
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, (void*)0);

}

