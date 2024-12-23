#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

// Output 
out vec3 worldPosition;
out vec3 reflectionVector;

// Uniforms
uniform mat4 MVP;        
uniform mat4 modelMatrix; 
uniform mat3 normalMatrix; 

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    vec4 worldPos = modelMatrix * vec4(vertexPosition, 1.0);
    worldPosition = worldPos.xyz;
    vec3 worldNormal = normalize(normalMatrix * vertexNormal);

    // reflection
    vec3 viewDir = normalize(-worldPosition); // Assume camera at (0,0,0)
    reflectionVector = reflect(viewDir, worldNormal);
    reflectionVector.y = -reflectionVector.y;

}
