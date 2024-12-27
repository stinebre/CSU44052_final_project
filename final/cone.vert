#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

// Output data, to be interpolated for each fragment
out vec3 color;
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;

uniform mat4 MVP;
uniform mat4 lightSpaceMatrix;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);
    
    // Pass vertex color to the fragment shader
    color = vertexColor;

    // World-space geometry 
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;

    // Light-space geometry
    fragPosLightSpace = lightSpaceMatrix * vec4(worldPosition, 1.0);
}


//out vec3 reflectionVector;
//out vec3 worldNormal;

    // reflection
    //vec3 viewDir = normalize(-worldPosition); // Assume camera at (0,0,0)
    //reflectionVector = reflect(viewDir, worldNormal);
    //reflectionVector.y = -reflectionVector.y;

