#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// GLTF model loader
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Camera
static glm::vec3 eye_center(0.0f, 100.0f, 800.0f);
static glm::vec3 lookat(0.0f, 0.0f, 0.0f);
static glm::vec3 up(0.0f, 1.0f, 0.0f);
static float FoV = 45.0f;
static float zNear = 100.0f;
static float zFar = 1500.0f; 
static glm::vec3 cameraPos(0.0f, 0.0f,  3.0f);

// View control
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static float viewDistance = 0.1f; //changed from 300.0f

// Lighting  
static glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
static glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

// Animation 
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

// timing - learnOpengl
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static GLuint LoadTextureTileBox(const char *texture_file_path)
{
	int w, h, channels;
	uint8_t *img = stbi_load(texture_file_path, &w, &h, &channels, 3);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// To tile textures on a box, we set wrapping to repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (img)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << texture_file_path << std::endl;
	}
	stbi_image_free(img);

	return texture;
}

struct box
{
    glm::vec3 position;
    glm::vec3 scale;

	GLfloat vertex_buffer_data[72] = {
		// Vertex definition for a canonical box
		// Front face
		-1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,

		// Back face
		1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,

		// Left face
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		-1.0f,

		// Right face
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		1.0f,
		-1.0f,
		1.0f,

		// Top face
		-1.0f,
		1.0f,
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		1.0f,

		// Bottom face
		-1.0f,
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		1.0f,
		1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		-1.0f,
		-1.0f,

	};

    GLfloat color_buffer_data[72] = {
        // Front, red
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // Back, yellow
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        // Left, green
        0.0f, 1.0f, 0.0f, 
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Right, cyan
        0.0f, 1.0f, 1.0f, 
        0.0f, 1.0f, 1.0f, 
        0.0f, 1.0f, 1.0f, 
        0.0f, 1.0f, 1.0f, 

        // Top, blue
        0.0f, 0.0f, 1.0f, 
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // Bottom, magenta
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 
        1.0f, 0.0f, 1.0f, 
        1.0f, 0.0f, 1.0f,  
    };

    GLuint index_buffer_data[36] = {        // 12 triangle faces of a box
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
        20, 22, 23, 
    };

    GLfloat uv_buffer_data[48] = {
        // Front
        512.0f/1024.0f,
        256.0f/768.0f,
        256.0f/1024.0f,
        256.0f/768.0f,
        256.0f/1024.0f,
        512.0f/768.0f,
        512.0f/1024.0f,
        512.0f/768.0f,

        // Back
        1.0f,
        256.0f/768.0f,
        768.0f/1024.0f,
        256.0f/768.0f,
        768.0f/1024.0f,
        512.0f/768.0f,
        1.0f,
        512.0f/768.0f,

        // Left
        768.0f/1024.0f,
        256.0f/768.0f,
        512.0f/1024.0f,
        256.0f/768.0f,
        512.0f/1024.0f,
        512.0f/768.0f,
        768.0f/1024.0f,
        512.0f/768.0f,

        // Right
        256.0f/1024.0f,
        256.0f/768.0f,
        0.0f,
        256.0f/768.0f,
        0.0f,
        512.0f/768.0f,
        256.0f/1024.0f,
        512.0f/768.0f,

        // Top 
        512.0f/1024.0f,
        0.0f,
        256.0f/1024.0f,
        0.0f,
        256.0f/1024.0f,
        256.0f/768.0f,
        512.0f/1024.0f,
        256.0f/768.0f,

        // Bottom 
        512.0f/1024.0f,
        512.0f/768.0f,
        256.0f/1024.0f,
        512.0f/768.0f,
        256.0f/1024.0f,
        1.0f,
        512.0f/1024.0f,
        1.0f,
    };

    // OpenGL buffers
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;

    // Shader variable IDs
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint programID;

	void initialize(glm::vec3 position, glm::vec3 scale)
	{
		// Define scale of the building geometry
		this->position = position;
		this->scale = scale;

		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
		// DONE:
		for (int i = 0; i < 72; ++i)
			color_buffer_data[i] = 1.0f;
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		// DONE: Create a vertex buffer object to store the UV data
		/*for (int i = 0; i < 24; ++i)
			uv_buffer_data[2 * i + 1] *= 3;*/
		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../final/skybox.vert", "../final/skybox.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");

		// DONE: Load a texture

		textureID = LoadTextureTileBox("../final/cloudySea.jpg");

		// DONE: Get a handle to texture sampler
		textureSamplerID = glGetUniformLocation(programID, "textureSampler");
	}

	void render(glm::mat4 cameraMatrix)
	{
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// DONE: Model transform
		// -----------------------
		glm::mat4 modelMatrix = glm::mat4();
		// Translate the box
		modelMatrix = glm::translate(modelMatrix, position);
		// Scale the box along each axis to make it look like a building
		modelMatrix = glm::scale(modelMatrix, scale);
		// -----------------------

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// DONE: Enable UV buffer and texture sampler
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// Set textureSampler to use texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);

		// Draw the box
		glDrawElements(
			GL_TRIANGLES,	 // mode
			36,				 // number of indices
			GL_UNSIGNED_INT, // type
			(void *)0		 // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		// glDisableVertexAttribArray(2);
	}

    void cleanup()
    {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteProgram(programID);
    }

};

// TODO: Cone geometry
struct spire
{
	glm::vec3 position;

	void initialize(glm::vec3 position)
	{
		this->position = position;
	}

	void render(glm::mat4 cameraMatrix) 
	{
		/*
		glm::mat4 modelMatrix = glm::mat4();
		modelMatrix = glm::translate(modelMatrix, position);
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		*/

	}
};

//TODO: Ocean simulation

//TODO: Model animation

//TODO: Lighting & Shadows

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, "final project", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	box skybox;
	skybox.initialize(glm::vec3(0, 0, 0), glm::vec3(100, 100, 100));

	box ground;
	ground.initialize(glm::vec3(0, 0, 0), glm::vec3(10, 1, 10));

	spire spire;
	spire.initialize(glm::vec3(0, 0, 0));

	// Camera setup
	eye_center.y = viewDistance;
	eye_center.x = viewDistance;
	eye_center.z = viewDistance;
	glm::float32 FoV = 45;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 1000.0f;
    glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;			// Animation time 
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;

	// Main loop
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;

		/*if (playAnimation) {
			time += deltaTime * playbackSpeed;
			bot.update(time);
		}*/

		// Rendering
		viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;
		skybox.render(vp);
		ground.render(vp);
		spire.render(vp);

		// FPS tracking 
		// Count number of frames over a few seconds and take average
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {		
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;
			
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "final project | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	skybox.cleanup();
	ground.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		viewAzimuth = 0.f;
		viewPolar = 0.f;
		eye_center.y = viewDistance * cos(viewPolar);
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
		std::cout << "Reset." << std::endl;
	}

	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar -= 0.1f;
		eye_center.y = viewDistance * cos(viewPolar);
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar += 0.1f;
		eye_center.y = viewDistance * cos(viewPolar);
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth -= 0.1f;
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth += 0.1f;
		eye_center.x = viewDistance * cos(viewAzimuth);
		eye_center.z = viewDistance * sin(viewAzimuth);
	}

	// TODO: free-roam camera 
	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	glm::vec3 cameraFront = eye_center - lookat;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        eye_center += cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        eye_center -= cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        eye_center -= glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        eye_center += glm::normalize(glm::cross(cameraFront, up)) * cameraSpeed;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
