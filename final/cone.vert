#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

out vec3 fragPosition;
out vec3 fragColor;
out vec3 fragNormal;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main() {
    fragPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    fragColor = vertexColor;
    fragNormal = normalize(normalMatrix * vertexNormal);

    gl_Position = MVP * vec4(vertexPosition, 1.0);
}
