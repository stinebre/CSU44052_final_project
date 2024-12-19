#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;

// Output data, to be interpolated for each fragment
out vec3 color;
out vec2 uv;

// Matrix for vertex transformation
uniform mat4 MVP;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);
    
    // Pass vertex color to the fragment shader
    color = vec3(0.0, 0.0, 1.0);

    uv = vec2(vertexPosition.x/2.0 + 0.5, vertexPosition.y/2.0 + 0.5);
}
