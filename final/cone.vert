#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;

uniform mat4 MVP;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position =  MVP * vec4(vertexPosition, 1); 
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;
    fragPosLightSpace = lightSpaceMatrix * vec4(worldPosition, 1.0);
}





