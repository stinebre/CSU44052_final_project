#version 330 core

layout(location = 0) in vec3 vertexPosition; // Vertex position
layout(location = 1) in vec2 vertexUV;       // Texture coordinates

out vec2 UV; // Pass texture coordinates to the fragment shader

uniform mat4 MVP; // Model-View-Projection matrix
uniform float time; // Time uniform to simulate wave movement

void main()
{
    // Apply the MVP transformation
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass the texture coordinates to the fragment shader
    UV = vertexUV;
}
