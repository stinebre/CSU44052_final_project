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
#include "camera.h"

#include <vector>
#include <iostream>
#include <list>
#define _USE_MATH_DEFINES
#include <math.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

// Camera - learnOpengl
Camera camera(glm::vec3(0.0f, 7.0f, 3.0f));
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Camera &camera, float deltaTime);

// Lighting  
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
static glm::vec3 lightIntensity = 5.0f * (8.0f * wave500 + 15.6f * wave600 + 18.4f * wave700);
static glm::vec3 lightPosition(-27.0f, 500.0f, -275.0f);
static glm::vec3 lightDir(-1.0f, -1.0f, -1.0f);

// Shadow mapping
static glm::vec3 lightUp(0, 0, 1);
static int shadowMapWidth = 1024;
static int shadowMapHeight = 1024;
glm::mat4 lightProjection, lightSpaceView, lightSpaceMatrix;

// Shadows
static float depthFoV = 100.0f; 
static float depthNear = 50.0f; 
static float depthFar = 750.0f; 

// Animation 
static bool playAnimation = true;
static float playbackSpeed = 2.0f;

// Timing 
float deltaTime = 0.0f; 
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

// Cubemap  - learnOpengl
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// Textured skybox
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

    GLuint index_buffer_data[36] = {
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
    GLuint boxprogID;

	void initialize(glm::vec3 position, glm::vec3 scale)
	{
		this->position = position;
		this->scale = scale;

		// VAOs and VBOs
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		for (int i = 0; i < 72; ++i)
			color_buffer_data[i] = 1.0f;
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Shaders and uniforms
		boxprogID = LoadShadersFromFile("../final/skybox.vert", "../final/skybox.frag");
		if (boxprogID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}
		mvpMatrixID = glGetUniformLocation(boxprogID, "MVP");

		// Texturing
		textureID = LoadTextureTileBox("../final/cloudySea.jpg");
		textureSamplerID = glGetUniformLocation(boxprogID, "textureSampler");
	}

	void render(glm::mat4 cameraMatrix)
	{
		glDepthMask(GL_FALSE); // Disable depth writes
		glUseProgram(boxprogID);
		glBindVertexArray(vertexArrayID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, camera.Position);
		modelMatrix = glm::scale(modelMatrix, scale);

		// Set uniforms
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Texturing
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);

		// Draw 
		glDrawElements(
			GL_TRIANGLES,	 // mode
			36,				 // number of indices
			GL_UNSIGNED_INT, // type
			(void *)0		 // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindVertexArray(0);
		glUseProgram(0);
		glDepthMask(GL_TRUE); 
	}

    void cleanup()
    {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteProgram(boxprogID);
    }

};

// Cone geometry
struct spire
{
    glm::vec3 position;
    glm::vec3 scale;

    static const int slices = 36;
    GLfloat vertex_buffer_data[slices * 3 + 3];
    GLfloat color_buffer_data[slices * 3 + 3];
    GLfloat normal_buffer_data[slices * 3 + 3];
    GLuint index_buffer_data[slices * 3];

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint normalBufferID;
    GLuint colorBufferID;

    GLuint lightPositionID;
    GLuint depthProgramID;
    GLuint lightSpaceMatrixID;
    GLuint depthlightSpaceMatrixID;
    GLuint shadowMapID;
	GLuint cameraPosID;
	GLuint lightDirID;

    GLuint modelMatrixID;
    GLuint normalMatrixID;
    GLuint mvpMatrixID;
    GLuint coneprogID;
    GLuint cubemapID;
	GLuint cubemapTextureUnit; 
    GLuint shadowMapTextureUnit;

   void initialize(glm::vec3 position, glm::vec3 scale, GLuint skyTexture)
    {
        this->position = position;
        this->scale = scale;
        this->cubemapID = skyTexture;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		shadowMapTextureUnit = 0; 
        cubemapTextureUnit = 1;

        // Step size for the circle
        float step = 2.0f * M_PI / slices;

        // Verticies
        vertex_buffer_data[0] = 0.0f;
        vertex_buffer_data[1] = 1.0f; 
        vertex_buffer_data[2] = 0.0f;
        int vbd_index = 3;
        for (int i = 0; i < slices; ++i)
        {
            float angle = i * step;
            vertex_buffer_data[vbd_index++] = cos(angle); 
            vertex_buffer_data[vbd_index++] = 0.0f;       
            vertex_buffer_data[vbd_index++] = sin(angle); 
        }

        // Colors
        for (int i = 0; i < slices * 3 + 6; ++i)
        {
            color_buffer_data[i] = 0.5f; // Light gray
        }

        // Normals
        normal_buffer_data[0] = 0.0f;
        normal_buffer_data[1] = 1.0f;
        normal_buffer_data[2] = 0.0f;
        int normal_index = 3;
        for (int i = 0; i < slices; ++i)
        {
            float angle = i * step;
            float next_angle = (i + 1) % slices * step;

            glm::vec3 current_vertex(cos(angle), 0.0f, sin(angle));
            glm::vec3 next_vertex(cos(next_angle), 0.0f, sin(next_angle));

            // Normal for the side (perpendicular to the surface)
            glm::vec3 side_normal = glm::normalize(glm::vec3(cos(angle), 0.5f, sin(angle)));
            normal_buffer_data[normal_index++] = side_normal.x;
            normal_buffer_data[normal_index++] = side_normal.y;
            normal_buffer_data[normal_index++] = side_normal.z;
        }

        // Side triangles
        int index = 0;
        for (int i = 0; i < slices; ++i)
        {
            index_buffer_data[index++] = 0;                      // Apex
            index_buffer_data[index++] = 1 + i;                 // Current base vertex
            index_buffer_data[index++] = 1 + (i + 1) % slices;  // Next base vertex
        }

        // Generate and bind VAO
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // Vertex Buffer
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        // Color Buffer
        glGenBuffers(1, &colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

        // Normal Buffer
        glGenBuffers(1, &normalBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

        // Index Buffer
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

        // Shaders
        coneprogID = LoadShadersFromFile("../final/cone.vert", "../final/cone.frag");
        if (coneprogID == 0)
        {
            std::cerr << "Failed to load shaders." << std::endl;
        }

        depthProgramID = LoadShadersFromFile("../final/depth.vert", "../final/depth.frag");
        if (depthProgramID == 0)
        {
            std::cerr << "Failed to load depth shaders." << std::endl;
        }

		// Texturing
		GLint cubemapSamplerID = glGetUniformLocation(coneprogID, "skybox");
        GLint shadowmapSamplerID = glGetUniformLocation(coneprogID, "shadowMap");
        if (cubemapSamplerID == -1 || shadowmapSamplerID == -1) {
            std::cerr << "Failed to get texture sampler uniform locations." << std::endl;
        }

        // Shader uniforms
        mvpMatrixID = glGetUniformLocation(coneprogID, "MVP");
        modelMatrixID = glGetUniformLocation(coneprogID, "modelMatrix");
        normalMatrixID = glGetUniformLocation(coneprogID, "normalMatrix");
        lightSpaceMatrixID = glGetUniformLocation(depthProgramID, "lightSpaceMatrix");
        depthlightSpaceMatrixID = glGetUniformLocation(coneprogID, "lightSpaceMatrix");
		cameraPosID = glGetUniformLocation(coneprogID, "cameraPos");
		lightDirID = glGetUniformLocation(coneprogID, "lightDir");
        if (mvpMatrixID == -1 || cameraPosID == -1 || lightDirID == -1) {
            std::cerr << "Failed to get uniform locations. (1)" << std::endl;
        }
        if(lightSpaceMatrixID == -1 || depthlightSpaceMatrixID == -1) {
            std::cerr << "Failed to get uniform locations. (2)" << std::endl;
        }

		glUseProgram(coneprogID);
        glUniform1i(cubemapSamplerID, cubemapTextureUnit);
        glUniform1i(shadowmapSamplerID, shadowMapTextureUnit);

        // Unbind VAO
        glBindVertexArray(0);
    }

    void render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix, GLuint depthMap)
    {
        glUseProgram(coneprogID);
        glBindVertexArray(vertexArrayID);

        // Positions
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

        // Model transformation
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
		lightPosition = glm::vec3(cameraMatrix * glm::vec4(lightPosition, 1.0f));

		// Shader uniforms
		glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));
		glm::vec3 cameraPos = camera.Position;
        glUniform3fv(lightDirID, 1, &lightDir[0]);
		glUniform3fv(cameraPosID, 1, &cameraPos[0]);
        glm::mat4 mvp = cameraMatrix * modelMatrix;
        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, &normalMatrix[0][0]);
        glUniform3fv(lightPositionID, 1, &lightPosition[0]);
        glUniformMatrix4fv(depthlightSpaceMatrixID, 1, GL_FALSE, &lightMatrix[0][0]);

		// Texturing
        glActiveTexture(GL_TEXTURE0 + shadowMapTextureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE0 + cubemapTextureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

        // Draw 
        glDrawElements(GL_TRIANGLES, slices * 6, GL_UNSIGNED_INT, 0);

        // Reset state
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void renderDepth(glm::mat4 lightSpaceMatrix) {
        glUseProgram(depthProgramID);
		glBindVertexArray(vertexArrayID);

		// Positions and Indices 
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// Shader uniforms
        glm::mat4 mvp = lightSpaceMatrix;
        glUniformMatrix4fv(lightSpaceMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Draw
        glDrawElements(GL_TRIANGLES, slices * 6, GL_UNSIGNED_INT, 0);

		// Reset
        glDisableVertexAttribArray(0);
        
    }

    void cleanup()
    {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &normalBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteProgram(coneprogID);
        glDeleteProgram(depthProgramID);
    }
};

// Fluid simulation (Fast Fourier Transform)
struct ocean {
    glm::vec3 position;
    glm::vec3 scale;

    static const int grid_size = 256;
    GLfloat vertex_buffer_data[grid_size * grid_size * 3];
    GLfloat uv_buffer_data[grid_size * grid_size * 2];
    GLuint index_buffer_data[(grid_size - 1) * (grid_size - 1) * 6];

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint uvBufferID;
    GLuint indexBufferID;

    GLuint oceanShaderID;
    GLuint fftShaderHorizontalID;
    GLuint fftShaderVerticalID;

    GLuint heightMapTexture;
    GLuint intermediateTexture;
    GLuint waveFBOHorizontal;
    GLuint waveFBOVertical;

    GLuint mvpMatrixID;
    GLuint heightMapID;
    
    GLuint lightDirID;
    GLuint lightPosID;
    GLuint ambientColorID;
	GLuint cameraPosID;
	GLuint depthProgramID;
    GLuint lightSpaceMatrixID;
    GLuint depthlightSpaceMatrixID;
	GLuint shadowMapTextureUnit;

    GLuint quadVAO, quadVBO;

    void initialize(glm::vec3 position, glm::vec3 scale) {
        this->position = position;
        this->scale = scale;

		shadowMapTextureUnit = 1;

        // Generate vertex and UV data for a grid
        int vertexIndex = 0, uvIndex = 0;
        for (int z = 0; z < grid_size; ++z) {
            for (int x = 0; x < grid_size; ++x) {
                vertex_buffer_data[vertexIndex++] = x - grid_size / 2.0f;
                vertex_buffer_data[vertexIndex++] = 0.0f;
                vertex_buffer_data[vertexIndex++] = z - grid_size / 2.0f;

                uv_buffer_data[uvIndex++] = x / float(grid_size - 1);
                uv_buffer_data[uvIndex++] = z / float(grid_size - 1);
            }
        }

        // Generate index buffer for the grid
        int index = 0;
        for (int z = 0; z < grid_size - 1; ++z) {
            for (int x = 0; x < grid_size - 1; ++x) {
                int topLeft = z * grid_size + x;
                int topRight = topLeft + 1;
                int bottomLeft = topLeft + grid_size;
                int bottomRight = bottomLeft + 1;

                index_buffer_data[index++] = topLeft;
                index_buffer_data[index++] = bottomLeft;
                index_buffer_data[index++] = topRight;
                index_buffer_data[index++] = topRight;
                index_buffer_data[index++] = bottomLeft;
                index_buffer_data[index++] = bottomRight;
            }
        }

        // Create VAO and buffers
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        glGenBuffers(1, &uvBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

        // Shaders
        oceanShaderID = LoadShadersFromFile("../final/water.vert", "../final/water.frag");
		depthProgramID = LoadShadersFromFile("../final/depth.vert", "../final/depth.frag");
        fftShaderHorizontalID = LoadShadersFromFile("../final/fft_horizontal.vert", "../final/fft_horizontal.frag");
        fftShaderVerticalID = LoadShadersFromFile("../final/fft_vertical.vert", "../final/fft_vertical.frag");


        // Shader uniforms
        mvpMatrixID = glGetUniformLocation(oceanShaderID, "MVP");
        heightMapID = glGetUniformLocation(oceanShaderID, "heightMap");
        lightDirID = glGetUniformLocation(oceanShaderID, "lightDir");
        lightPosID = glGetUniformLocation(oceanShaderID, "lightPos");
        ambientColorID = glGetUniformLocation(oceanShaderID, "ambientColor");
		cameraPosID = glGetUniformLocation(oceanShaderID, "cameraPos");
		lightSpaceMatrixID = glGetUniformLocation(depthProgramID, "lightSpaceMatrix");
        depthlightSpaceMatrixID = glGetUniformLocation(oceanShaderID, "lightSpaceMatrix");

        // FBO and texturing
        setupFBO();
        setupFullScreenQuad();
    }

    void setupFBO() {
        glGenTextures(1, &heightMapTexture);
		glBindTexture(GL_TEXTURE_2D, heightMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, grid_size, grid_size, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenTextures(1, &intermediateTexture);
		glBindTexture(GL_TEXTURE_2D, intermediateTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, grid_size, grid_size, 0, GL_RED, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenFramebuffers(1, &waveFBOHorizontal);
		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOHorizontal);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTexture, 0); 

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOHorizontal);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Horizontal FBO not complete!" << std::endl;
		}

		glGenFramebuffers(1, &waveFBOVertical);
		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOVertical);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, heightMapTexture, 0); 

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOVertical);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Vertical FBO not complete!" << std::endl;
		}


		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
    }

    void setupFullScreenQuad() {
        GLfloat quadVertices[] = {
            // Positions   // UVs
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f,  1.0f,  1.0f, 1.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glBindVertexArray(quadVAO);

        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0); 
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

		// UV
        glEnableVertexAttribArray(1); 
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }

    void fftHorizontalPass(float time, int numPass) {
		glUseProgram(fftShaderHorizontalID);

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOHorizontal);
		 
		// Texturing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightMapTexture); 

		// Shader uniforms
		glUniform1i(glGetUniformLocation(fftShaderHorizontalID, "inputTexture"), 0);
		glUniform1i(glGetUniformLocation(fftShaderHorizontalID, "passNumber"), numPass);
		glUniform1f(glGetUniformLocation(fftShaderHorizontalID, "time"), time);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void fftVerticalPass(float time, int numPass) {
		glUseProgram(fftShaderVerticalID);

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOVertical);

		// Texturing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, intermediateTexture); 
		glUniform1i(glGetUniformLocation(fftShaderVerticalID, "horizontalPassTexture"), 0);

		// Shader uniforms
		glUniform1i(glGetUniformLocation(fftShaderVerticalID, "width"), grid_size);
		glUniform1i(glGetUniformLocation(fftShaderVerticalID, "passNumber"), numPass);
		glUniform1f(glGetUniformLocation(fftShaderVerticalID, "time"), time);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    void render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix, GLuint depthMap, float time) {

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOHorizontal);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, waveFBOVertical);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Determines step size
		int numPasses = int(log2(float(grid_size)));
    	for (int pass = 0; pass < numPasses; ++pass) {
			fftHorizontalPass(time, pass);
			fftVerticalPass(time, pass);
		}

		// Rendering using height map texture
        glUseProgram(oceanShaderID);
        glBindVertexArray(vertexArrayID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// Shader uniforms
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, scale);
		glm::mat4 mvpMatrix = cameraMatrix * modelMatrix;

		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);
		glUniformMatrix4fv(depthlightSpaceMatrixID, 1, GL_FALSE, &lightMatrix[0][0]);

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
		glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.5f);
		glm::vec3 cameraPos = camera.Position;
		glUniform3fv(lightDirID, 1, &lightDir[0]);
		glUniform3fv(ambientColorID, 1, &ambientColor[0]);
		glUniform3fv(cameraPosID, 1, &cameraPos[0]);

		// Texturing
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightMapTexture);
		glUniform1i(heightMapID, 0);

		glActiveTexture(GL_TEXTURE0 + shadowMapTextureUnit);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		GLint shadowMapLocation = glGetUniformLocation(oceanShaderID, "shadowMap");
		glUniform1i(shadowMapLocation, shadowMapTextureUnit);

		// Final draw
        glDrawElements(GL_TRIANGLES, (grid_size - 1) * (grid_size - 1) * 6, GL_UNSIGNED_INT, nullptr);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
    }

	void renderDepth(glm::mat4 lightSpaceMatrix) {
        glUseProgram(depthProgramID);
		glBindVertexArray(vertexArrayID);

		// Positions and Indices 
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		// Shader uniforms
        glm::mat4 mvp = lightSpaceMatrix;
        glUniformMatrix4fv(lightSpaceMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// Draw
        glDrawElements(GL_TRIANGLES, (grid_size - 1) * (grid_size - 1) * 6, GL_UNSIGNED_INT, nullptr);

		// Reset
        glDisableVertexAttribArray(0);
        
    }

    void cleanup() {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &uvBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteProgram(oceanShaderID);
        glDeleteProgram(fftShaderHorizontalID);
        glDeleteProgram(fftShaderVerticalID);
        glDeleteTextures(1, &heightMapTexture);
        glDeleteTextures(1, &intermediateTexture);
        glDeleteFramebuffers(1, &waveFBOHorizontal);
        glDeleteFramebuffers(1, &waveFBOVertical);
        glDeleteBuffers(1, &quadVBO);
        glDeleteVertexArrays(1, &quadVAO);
    }
};

//Model animation
struct MyBot {
	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint jointMatricesID;
	GLuint lightPositionID;
	GLuint lightIntensityID;
	GLuint programID;

	GLuint textureSamplerID;
	GLuint textureID;
	tinygltf::Model model;

	// Each VAO corresponds to each mesh primitive in the GLTF model
	struct PrimitiveObject {
		GLuint vao;
		std::map<int, GLuint> vbos;
	};
	std::vector<PrimitiveObject> primitiveObjects;

	// Skinning 
	struct SkinObject {
		// Transforms the geometry into the space of the respective joint
		std::vector<glm::mat4> inverseBindMatrices;  

		// Transforms the geometry following the movement of the joints
		std::vector<glm::mat4> globalJointTransforms;

		// Combined transforms
		std::vector<glm::mat4> jointMatrices;
	};
	std::vector<SkinObject> skinObjects;

	// Animation 
	struct SamplerObject {
		std::vector<float> input;
		std::vector<glm::vec4> output;
		int interpolation;
	};
	struct ChannelObject {
		int sampler;
		std::string targetPath;
		int targetNode;
	}; 
	struct AnimationObject {
		std::vector<SamplerObject> samplers;	// Animation data
	};
	std::vector<AnimationObject> animationObjects;

	glm::mat4 getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f); 

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
			if (node.translation.size() == 3) {
				transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
			}
			if (node.rotation.size() == 4) {
				glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				transform *= glm::mat4_cast(q);
			}
			if (node.scale.size() == 3) {
				transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
			}
		}
		return transform;
	}

	void computeLocalNodeTransform(const tinygltf::Model& model, 
		int nodeIndex, 
		std::vector<glm::mat4> &localTransforms)
	{
		// DONE: your code here
		// ---------------------------------------
		const tinygltf::Node& node = model.nodes[nodeIndex];
		localTransforms[nodeIndex] = getNodeTransform(node);
		for (int childIndex : node.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}

	}

	void computeGlobalNodeTransform(const tinygltf::Model& model, 
		const std::vector<glm::mat4> &localTransforms,
		int nodeIndex, const glm::mat4& parentTransform, 
		std::vector<glm::mat4> &globalTransforms)
	{
		// DONE: your code here
		// ----------------------------------------
		const tinygltf::Node& node = model.nodes[nodeIndex];
		globalTransforms.push_back(parentTransform * localTransforms[nodeIndex]);
		for (int childIndex : node.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, parentTransform * localTransforms[nodeIndex], globalTransforms);
		}
	}

	std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model) {
		std::vector<SkinObject> skinObjects;

		// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject skinObject;

			const tinygltf::Skin &skin = model.skins[i];

			// Read inverseBindMatrices
			const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
			assert(accessor.type == TINYGLTF_TYPE_MAT4);
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			const float *ptr = reinterpret_cast<const float *>(
            	buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);
			
			skinObject.inverseBindMatrices.resize(accessor.count);
			for (size_t j = 0; j < accessor.count; j++) {
				float m[16];
				memcpy(m, ptr + j * 16, 16 * sizeof(float));
				skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
			}

			assert(skin.joints.size() == accessor.count);

			skinObject.globalJointTransforms.resize(skin.joints.size());
			skinObject.jointMatrices.resize(skin.joints.size());

			// DONE: your code here to compute joint matrices
			// ----------------------------------------------

       		std::vector<glm::mat4> localTransforms(skin.joints.size());
       		computeLocalNodeTransform(model, skin.joints[0], localTransforms);

			glm::mat4 parentTransform(1.0f);
			computeGlobalNodeTransform(model, localTransforms, skin.joints[0], parentTransform, skinObject.globalJointTransforms);

			for(int j = 0; j < skin.joints.size(); j++) {
				skinObject.jointMatrices[j] = skinObject.globalJointTransforms[skin.joints[j]]*skinObject.inverseBindMatrices[j];
			}

			skinObjects.push_back(skinObject);
		}
		return skinObjects;
	}

	int findKeyframeIndex(const std::vector<float>& times, float animationTime) 
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model) 
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;
			
			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}
				
				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);
				
				// Output values
				samplerObject.output.resize(outputAccessor.count);
				
				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);			
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void updateAnimation(
		const tinygltf::Model &model, 
		const tinygltf::Animation &anim, 
		const AnimationObject &animationObject, 
		float time,
		std::vector<glm::mat4> &nodeTransforms) 
	{
		// There are many channels so we have to accumulate the transforms 
		for (const auto &channel : anim.channels) {
			
			int targetNodeIndex = channel.target_node;
			const auto &sampler = anim.samplers[channel.sampler];
			
			// Access output (value) data for the channel
			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			// Calculate current animation time (wrap if necessary)
			const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
			float animationTime = fmod(time, times.back());
			
			// DONE: Find a keyframe for getting animation data 
			// ----------------------------------------------------------
			int keyframeIndex = findKeyframeIndex(times, animationTime);  

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			// -----------------------------------------------------------
			// TODO: Add interpolation for smooth animation
			// -----------------------------------------------------------
			if (channel.target_path == "translation") {
				glm::vec3 translation0, translation1;
				memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 translation = translation0;
				nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
			} else if (channel.target_path == "rotation") {
				glm::quat rotation0, rotation1;
				memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));

				glm::quat rotation = rotation0;
				nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
			} else if (channel.target_path == "scale") {
				glm::vec3 scale0, scale1;
				memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));

				glm::vec3 scale = scale0;
				nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
			}
		}
	}

	void updateSkinning(const std::vector<glm::mat4> &nodeTransforms) {

		// DONE: Recompute joint matrices 
		// -------------------------------------------------
		for (size_t i = 0; i < skinObjects.size(); i++) {
			SkinObject& skinObject = skinObjects[i];
			const tinygltf::Skin& skin = model.skins[i];

			for (size_t j = 0; j < skinObject.jointMatrices.size(); ++j) {
				skinObject.jointMatrices[j] = nodeTransforms[j] * skinObject.inverseBindMatrices[j];
			}
		}

	}

	void changeBotPosition(glm::vec3 newPosition) {
		int rootNodeIndex = model.scenes[model.defaultScene].nodes[0];
		tinygltf::Node &rootNode = model.nodes[rootNodeIndex];

		// Update translation of the root node
		rootNode.translation = {newPosition.x, newPosition.y, newPosition.z};

		// Recompute transforms
		std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
		computeLocalNodeTransform(model, rootNodeIndex, localTransforms);

		glm::mat4 parentTransform = glm::mat4(1.0f);
		std::vector<glm::mat4> globalTransforms;
		computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, parentTransform, globalTransforms);

		// Update skinning with new transforms
		updateSkinning(globalTransforms);
	}

	void update(float time) {
		//return;

		// DONE: your code here
		// -------------------------------------------------
		if (model.animations.size() > 0) {
			const tinygltf::Skin& skin = model.skins[0];
			const tinygltf::Animation& animation = model.animations[0];
			const AnimationObject& animationObject = animationObjects[0];
			std::vector<glm::mat4> nodeTransforms(skin.joints.size());

			for (size_t i = 0; i < nodeTransforms.size(); ++i) {
				nodeTransforms[i] = glm::mat4(1.0);
			}

			updateAnimation(model, animation, animationObject, time, nodeTransforms);
			glm::mat4 parentTransform(1.0f);
			std::vector<glm::mat4> globalNodeTransform(0);
			computeGlobalNodeTransform(model, nodeTransforms, skin.joints[0], parentTransform, globalNodeTransform);

			updateSkinning(globalNodeTransform);
		}
	}

	bool loadModel(tinygltf::Model &model, const char *filename) {
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			std::cout << "ERR: " << err << std::endl;
		}

		if (!res)
			std::cout << "Failed to load glTF: " << filename << std::endl;
		else
			std::cout << "Loaded glTF: " << filename << std::endl;

		return res;
	}

	void initialize() {
		// Modify your path if needed
		if (!loadModel(model, "../final/model/bot/bot.gltf")) {
			return;
		}

		// Prepare buffers for rendering 
		primitiveObjects = bindModel(model);

		// Prepare joint matrices
		skinObjects = prepareSkinning(model);

		// Prepare animation data 
		animationObjects = prepareAnimation(model);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../final/bot.vert", "../final/bot.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for GLSL variables
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
		jointMatricesID = glGetUniformLocation(programID,"u_jointMatrix"); 
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");

		textureID = LoadTextureTileBox("../final/skin.png");
		textureSamplerID = glGetUniformLocation(programID, "textureSampler");
	}

	void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {

		std::map<int, GLuint> vbos;
		for (size_t i = 0; i < model.bufferViews.size(); ++i) {
			const tinygltf::BufferView &bufferView = model.bufferViews[i];

			int target = bufferView.target;
			
			if (bufferView.target == 0) { 
				// The bufferView with target == 0 in our model refers to 
				// the skinning weights, for 25 joints, each 4x4 matrix (16 floats), totaling to 400 floats or 1600 bytes. 
				// So it is considered safe to skip the warning.
				//std::cout << "WARN: bufferView.target is zero" << std::endl;
				continue;
			}

			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, bufferView.byteLength,
						&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
			
			vbos[i] = vbo;
		}

		// Each mesh can contain several primitives (or parts), each we need to 
		// bind to an OpenGL vertex array object
		for (size_t i = 0; i < mesh.primitives.size(); ++i) {

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			GLuint vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			for (auto &attrib : primitive.attributes) {
				tinygltf::Accessor accessor = model.accessors[attrib.second];
				int byteStride =
					accessor.ByteStride(model.bufferViews[accessor.bufferView]);
				glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

				int size = 1;
				if (accessor.type != TINYGLTF_TYPE_SCALAR) {
					size = accessor.type;
				}

				int vaa = -1;
				if (attrib.first.compare("POSITION") == 0) vaa = 0;
				if (attrib.first.compare("NORMAL") == 0) vaa = 1;			
				if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
				if (attrib.first.compare("JOINTS_0") == 0) vaa = 3;
				if (attrib.first.compare("WEIGHTS_0") == 0) vaa = 4;
				if (vaa > -1) {
					glEnableVertexAttribArray(vaa);
					glVertexAttribPointer(vaa, size, accessor.componentType,
										accessor.normalized ? GL_TRUE : GL_FALSE,
										byteStride, BUFFER_OFFSET(accessor.byteOffset));
				} else {
					std::cout << "vaa missing: " << attrib.first << std::endl;
				}
			}

			// Record VAO for later use
			PrimitiveObject primitiveObject;
			primitiveObject.vao = vao;
			primitiveObject.vbos = vbos;
			primitiveObjects.push_back(primitiveObject);

			glBindVertexArray(0);
		}
	}

	void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects, 
						tinygltf::Model &model,
						tinygltf::Node &node) {
		// Bind buffers for the current mesh at the node
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}

		// Recursive into children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}

	std::vector<PrimitiveObject> bindModel(tinygltf::Model &model) {
		std::vector<PrimitiveObject> primitiveObjects;

		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}

		return primitiveObjects;
	}

	void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {
		
		for (size_t i = 0; i < mesh.primitives.size(); ++i) 
		{
			GLuint vao = primitiveObjects[i].vao;
			std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

			glBindVertexArray(vao);

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

			glDrawElements(primitive.mode, indexAccessor.count,
						indexAccessor.componentType,
						BUFFER_OFFSET(indexAccessor.byteOffset));

			glBindVertexArray(0);
		}
	}

	void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
						tinygltf::Model &model, tinygltf::Node &node) {
		// Draw the mesh at the node, and recursively do so for children nodes
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}
		for (size_t i = 0; i < node.children.size(); i++) {
			drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}
	void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
				tinygltf::Model &model) {
		// Draw all nodes
		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}
	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);
		
		// Set camera
		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);

		for (size_t i = 0; i < skinObjects.size(); i++) {
			const SkinObject& skin = skinObjects[i];
			glUniformMatrix4fv(jointMatricesID, skin.jointMatrices.size(), GL_FALSE, glm::value_ptr(skin.jointMatrices[0]));
		}

		// Set light data 
		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

		// Draw the GLTF model
		drawModel(primitiveObjects, model);
	}

	void cleanup() {
		glDeleteProgram(programID);
	}
}; 

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

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	box skybox;
	skybox.initialize(camera.Position, glm::vec3(100, 100, 100));

	std::vector<std::string> faces = {
		"../final/right.jpg", "../final/left.jpg",
		"../final/top.jpg", "../final/bottom.jpg",
		"../final/front.jpg", "../final/back.jpg"
	};
	GLuint cubemapTexture = loadCubemap(faces);
	spire spire;
	spire.initialize(glm::vec3(0, 0.01, -30), glm::vec3(3, 30, 3), cubemapTexture);

	MyBot k;
	k.initialize();

	ocean tile1;
    tile1.initialize(glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// Create and activate FBO
    GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check for completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Camera setup
	glm::float32 FoV = 45;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 1000.0f;
    glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, zNear, zFar);

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

		processInput(window, camera, deltaTime);

		// view/projection transformations
        glm::mat4 viewMatrix = camera.GetViewMatrix();
	
		// Rendering
		glm::mat4 vp = projectionMatrix * viewMatrix;
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 100.0f);
        glm::mat4 lightView = glm::lookAt(lightPosition, lightPosition+lightDir, camera.Up); 
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		// Render objects for depth
		spire.renderDepth(lightSpaceMatrix);
		tile1.renderDepth(lightSpaceMatrix);

		// Unbind FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        spire.render(vp, lightSpaceMatrix, depthMap);
		tile1.render(vp, lightSpaceMatrix, depthMap, currentTime);

        skybox.render(vp);

		if (playAnimation) {
			time += deltaTime * playbackSpeed;
			k.update(time);
		}
		k.render(vp);

		// FPS tracking 
		// Count number of frames over a few seconds and take average
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {		
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;
			
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Final Project | Frames Per Second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	skybox.cleanup();
	spire.cleanup();
	tile1.cleanup();
	k.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Free-roam camera
void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}