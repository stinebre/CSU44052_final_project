#version 330 core

layout(location = 0) in vec2 vertexPosition; // Input vertex position (NDC)
layout(location = 1) in vec2 vertexUV;       // Input texture coordinates

out vec2 UV; // Pass the texture coordinates to the fragment shader

void main()
{
    // Pass the texture coordinates to the fragment shader
    UV = vertexUV;

    // Set the position of the vertex in normalized device coordinates
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
