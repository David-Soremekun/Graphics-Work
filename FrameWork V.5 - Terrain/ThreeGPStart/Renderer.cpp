#include "Renderer.h"
#include "Camera.h"
#include "ImageLoader.h"

Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	//glDeleteBuffers(1, &m_VAO);
	for (const GLuint VAO : m_VAO) {
		glDeleteBuffers(1, &VAO);
	}
	
}
glm::mat4 Renderer::translation(glm::mat4 modelMatrix, glm::vec3 translation)
{
	glm::mat4 ReturnMat;
	ReturnMat=glm::translate(modelMatrix, translation);
	return ReturnMat;
}

glm::mat4 Renderer::rotation(glm::mat4 modelMatrix, GLuint angle, glm::vec3 axis)
{
	glm::mat4 ReturnedMat;
	ReturnedMat = glm::rotate(modelMatrix ,static_cast<float>(angle), axis);
	return ReturnedMat;
}


// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Checkbox("Cull Face", &m_cullFace);

	

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram(std::string ShaderName, std::string FragName, GLuint &m_program)
{
	if (m_program)
		glDeleteProgram(m_program);

	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/" + ShaderName + ".vert") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/" + FragName + ".frag") };
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

bool Renderer::CreateTerrain(int numberOfCellsX, int numberOfCellsZ, float sizeX, float sizeZ, const std::string& textureFilename)
{
	
	Helpers::ImageLoader heightMap;
	if (!heightMap.Load("Data\\Heightmaps\\testMap.bmp"))
		std::cout<<"Did Not Load";


	// need offset values for sizex and sizez (starting at -sizex/z divided by 2
	float OffsetForZ = -sizeZ / 2;
	float OffsetForX; // Create the Starting Offsets to be incremented to the the other side from midpoint

	std::vector<glm::vec3> vertexVector;
	std::vector<glm::vec3> colourVector;
	std::vector<glm::vec3> normalVector;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> elementVector;
	


	float vertexXtoImage =(float)heightMap.Width() / (numberOfCellsX+1);
	float vertexZtoImage =(float)heightMap.Height() / (numberOfCellsZ+1);

	int imageX, imageZ;
	size_t Offset;
	BYTE* heightData = heightMap.GetData();
	BYTE height;
	
	
	
	// create vertices
	for (size_t z = 0; z < numberOfCellsZ + 1; z++)
	{
		OffsetForX = -sizeX / 2;
		for (size_t x = 0; x < numberOfCellsX + 1; x++)
		{
			imageX = vertexXtoImage * x;
			imageZ = vertexZtoImage * z;

			Offset = ((size_t)imageX + (size_t)imageZ * heightMap.Width()) * 4;
			height = heightData[Offset];
			
			vertexVector.push_back(glm::vec3(OffsetForX, ((float)height * 0.015) + GenerateNoise(x,z), OffsetForZ));
			

			OffsetForX += sizeX / numberOfCellsX;
			

			

			normalVector.push_back({ 0,0,0 });
			
			// add by offset
			// increment offset
			// push back
		}
		OffsetForZ += sizeZ / numberOfCellsZ;

	}
	OffsetForZ = 0;

	// create Elements
	bool toggleForDiamondPattern=false;
	for (size_t z = 0; z < numberOfCellsZ; z++) {
		for (size_t x = 0; x < numberOfCellsX; x++) {

			if (toggleForDiamondPattern) 
			{
				elementVector.push_back(x + 1 + OffsetForZ); // 1
				elementVector.push_back(x + OffsetForZ); // 0
				elementVector.push_back(numberOfCellsX + x + 1 + OffsetForZ); // 2

				elementVector.push_back(numberOfCellsX + x + 1 + OffsetForZ); // 2
				elementVector.push_back(numberOfCellsX + x + 2 + OffsetForZ); // 3
				elementVector.push_back(x + 1 + OffsetForZ); // 1
				

			}
			else 
			{
				elementVector.push_back(x + 1 + OffsetForZ); // 1
				elementVector.push_back(x + OffsetForZ); // 0
				elementVector.push_back(numberOfCellsX + x + 2 + OffsetForZ); // 3

				elementVector.push_back(numberOfCellsX + x + 2 + OffsetForZ); // 3
				elementVector.push_back(x + OffsetForZ); // 0
				elementVector.push_back(numberOfCellsX + x + 1 + OffsetForZ); // 2
				
			}
			toggleForDiamondPattern = !toggleForDiamondPattern;
		}
		toggleForDiamondPattern = !toggleForDiamondPattern;
		OffsetForZ += numberOfCellsX + 1;
	}
	//Create Normals
	
	size_t length = elementVector.size();

	for (size_t i = 0; i < length; i += 3)
	{
		glm::vec3 normalVec =glm::normalize( glm::cross(vertexVector[elementVector[i+1]] - vertexVector[elementVector[i]], vertexVector[elementVector[i+2]] - vertexVector[elementVector[i]]));
		//glm::vec3 normalVec = glm::cross(elementVector[i + 1] - elementVector[i], elementVector[i+2] - elementVector[i]);
		normalVector[elementVector[i]] += normalVec;
		normalVector[elementVector[i+1]] += normalVec;
		normalVector[elementVector[i+2]] += normalVec;
		/*std::cout << "element" + elementVector;*/
	}

	/*for (size_t i = 0; i < normalVector.size(); i++)
	{
		normalVector[i]=glm::normalize(normalVector[i]);
	}*/


	//Create Texture
	float xTex=0, yTex=0;
	for (size_t x = 0; x <= numberOfCellsX; x++)
	{
		for (size_t y = 0; y <= numberOfCellsZ; y++)
		{
			//vertexVector.push_back(glm::vec3(x * 10, 0, y * 10));
			colourVector.push_back({0, 1, 0});

			
			// For the U coord
			xTex = x / (numberOfCellsX - 1.0f);
			
			
			// For the V coord
			 yTex = y / (numberOfCellsZ - 1.0f);

			

			textureCoords.push_back(glm::vec2(xTex,yTex));
			//textureCoords.push_back(glm::vec2(x,y));
		}
	}

	// Heightmap Creation
	

	// Noise


	
	// Positions VBO
	GLuint PositionVBO;
	glGenBuffers(1, &PositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PositionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexVector.size(), vertexVector.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	//  Normals VBO
	GLuint normalVBO;
	glGenBuffers(1, &normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normalVector.size(), normalVector.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	
	//  Textures VBO
	GLuint textureVBO;
	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Element VBO
	GLuint elementEBO;
	m_numElements.push_back(elementVector.size());
	glGenBuffers(1, &elementEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementVector.size(), elementVector.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// General VAO for Terrain
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, PositionVBO);
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
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
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

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer
	(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	glBindVertexArray(0);
	
	
	m_VAO.push_back(VAO);


	Helpers::ImageLoader Image;
	if (Image.Load("Data\\Textures\\" + textureFilename + ".dds"))
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,Image.Width(), Image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,Image.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	

	return true;
}

float Renderer::GenerateNoise(int x, int y)
{
	int n = x * y * 37;
	n = (n >> 13) ^ n;
	int n2=(n*(n*n*60943+19990303)+1376312589)&0x7ffffff;


	return 1.0f-((float)n2/1073741824.0f);
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram("vertex_shader","fragment_shader",m_program))
		return false;
	if (!CreateProgram("vertexshader_skybox", "fragshader_skybox", m_program2))
		return false;
	// Helpers has an object for loading 3D geometry, supports most types
	
	//// E.g. Load in the jeep
	//Helpers::ModelLoader loader;
	//if (!loader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
	//	return false;

	Helpers::ModelLoader loaderBoat;
	/*if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\hull.obj"))
		return false;
	if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\wing_right.obj"))
		return false;
	if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\wing_left.obj"))
		return false;
	if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\propeller.obj"))
		return false;
	if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\gun_base.obj"))
		return false;
	if (!loaderBoat.LoadFromFile("Data\\Models\\AquaPig\\gun.obj"))
		return false;*/
	
	
		

	if (CreateTerrain(50, 50, 10, 10, "dirt_earth-n-moss_df_"))
	{
		// do nothing
	}

	for (const Helpers::Mesh& mesh : loaderBoat.GetMeshVector()) {
		GLuint newElementNum = mesh.elements.size();
		m_numElements.push_back(newElementNum);
		
		
		GLuint positionsVBO;
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		

		GLuint normalVBO;
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		

		GLuint textureVBO;
		glGenBuffers(1, &textureVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		GLuint elementEBO;
		
		glGenBuffers(1, &elementEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * newElementNum, mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// TODO: create a VBA to wrap everything and specify locations in the shaders
		GLuint newVAO;
		glGenVertexArrays(1, &newVAO);
		glBindVertexArray(newVAO);

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
		
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
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
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer
		(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
		glBindVertexArray(0);

		m_VAO.push_back(newVAO);

		
		glm::vec3 minValues{-10,-10,10};
		glm::vec3 maxValues{10,10,-10};

		int numberOfVertsInX = 20;
		int numberOfVertsInY = 20;

		return true;

	}
	/*Helpers::ImageLoader boatTexture;
	if (!boatTexture.Load("Data\\Models\\AquaPig\\aqua_pig_2K.png"))
		return false;

	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, boatTexture.Width(), boatTexture.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, boatTexture.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);*/


	// Now we can loop through all the mesh in the loaded model:
	//for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	//{
	//	// We can extract from the mesh via:
	//	//mesh.vertices  - a vector of glm::vec3 (3 floats) giving the position of each vertex
	//	

	//	//mesh.elements - a vector of unsigned ints defining which vertices make up each triangle

	//	
	//	// TODO: create VBO for the vertices and a EBO for the elements
	//	
	//	GLuint positionsVBO;
	//	glGenBuffers(1, &positionsVBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	
	//	GLuint normalVBO;
	//	glGenBuffers(1, &normalVBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
	//	glBindBuffer(GL_ARRAY_BUFFER,0);


	//	int texWidth, texHeight;

	//	GLuint textureVBO;
	//	glGenBuffers(1, &textureVBO);
	//	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	//	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//	GLuint elementEBO;
	//	m_numElements = mesh.elements.size();
	//	glGenBuffers(1, &elementEBO);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_numElements, mesh.elements.data(), GL_STATIC_DRAW);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//	// TODO: create a VBA to wrap everything and specify locations in the shaders

	//	glGenVertexArrays(1, &m_VAO);
	//	glBindVertexArray(m_VAO);

	//	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	//	glEnableVertexAttribArray(0);
	//	glVertexAttribPointer
	//	(
	//		0,
	//		3,
	//		GL_FLOAT,
	//		GL_FALSE,
	//		0,
	//		(void*)0
	//	);
	//	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer
	//	(
	//		1,
	//		3,
	//		GL_FLOAT,
	//		GL_FALSE,
	//		0,
	//		(void*)0 
	//	);
	//	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	//	glEnableVertexAttribArray(2);
	//	glVertexAttribPointer
	//	(
	//		2,
	//		2,
	//		GL_FLOAT,
	//		GL_FALSE,
	//		0,
	//		(void*)0
	//	);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	//	glBindVertexArray(0);
	//}

	//Helpers::ImageLoader texLoader;
	//if (!texLoader.Load("Data\\Models\\Jeep\\jeep_army.jpg"))
	//	return false;
	//
	//GLuint texture;
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texLoader.Width(), texLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texLoader.GetData());
	//glGenerateMipmap(GL_TEXTURE_2D);

	

	return true;
}





// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);

	if (m_cullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(75.f), aspect_ratio, 1.0f, 2000.f);

	//glm::mat4 model_xform = glm::translate(glm::mat4(1),glm::vec3(1,2,3));
	
		

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;


	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program2);

	// Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program2, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	size_t count{ 0 };

	//glm::vec3 transformArray[6];
	//transformArray[0] = { 0,0,0 };
	//transformArray[1] = { -2.231,0.272,-2.663 };
	//transformArray[2] = { 2.231,0.272,-2.663 };
	//transformArray[3] = { 0,1.395,-3.616 };
	//transformArray[4] = { 0,0.569,-1.866 };
	//transformArray[5] = { 0,1.506,0.644 };
	////transformArray[3] = rotation(transformArray, 90, {0.0, 1.0, 0.0});
	//
	//
	//for (GLuint VAO: m_VAO) {
	//	glm::mat4 model_xform = glm::mat4(1);
	//	glm::mat4 rotateMat = glm::mat4(1);
	//	static bool rotating=true;
	//	static float rotX = 2;
	//	
	//	//model_xform = glm::rotate(model_xform, rotX, glm::vec3(1, 0, 0));
	//	if (count != 0)
	//	{

	//		if (count == 5)
	//		{
	//			model_xform = translation(model_xform, transformArray[count - 1]);
	//			model_xform = translation(model_xform, transformArray[count]);
	//		}
	//		else
	//		{
	//			model_xform = translation(model_xform, transformArray[count]);
	//		}
	//		if (rotating)
	//		{
	//			if (count == 3)
	//			{
	//				rotateMat = glm::rotate(rotateMat, glm::radians(rotX), glm::vec3(1, 0, 0));
	//				rotateMat = glm::rotate(rotateMat, glm::radians(propRot), glm::vec3(0, 1, 0));
	//				rotateMat = glm::rotate(rotateMat, 270.f, glm::vec3(0, 1, 0));
	//			}model_xform = model_xform * rotateMat;

	//		}

	//		if (rotX > glm::two_pi<float>())
	//		{
	//			rotX = 0;

	//			rotating = !rotating;
	//		}
	//	}
	//	// Send the model matrix to the shader in a uniform
	//	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	//	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	glActiveTexture(GL_TEXTURE0);
	

	
	glBindTexture(GL_TEXTURE_2D, texture);

	GLuint vg = glGetUniformLocation(m_program, "sampler_tex");

	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

	//	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	//	glBindVertexArray(VAO);
	//	count++;
	//	glDrawElements(GL_TRIANGLES,m_numElements[count], GL_UNSIGNED_INT, (void*)0);
	//	// TODO: Send the combined matrix to the shader in a uniform

	//	
	//}
	//propRot += 0.5;

	glm::mat4 model_xform = glm::mat4(1);
	GLuint model_xform_id = glGetUniformLocation(m_program2, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	glBindVertexArray(m_VAO[0]);
	glDrawElements(GL_TRIANGLES, m_numElements[0], GL_UNSIGNED_INT, (void*)0);
	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
}

