#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;

out vec2 fragUV;

void main() {
    fragUV = vertexUV;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}
