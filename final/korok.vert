#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;

uniform mat4 MVP;
uniform mat4 u_jointMatrix[25]; 

void main() {

    mat4 skinMatrix = 
    a_weight.x * u_jointMatrix[int(a_joint.x)] 
    + a_weight.y * u_jointMatrix[int(a_joint.y)] 
    + a_weight.z * u_jointMatrix[int(a_joint.z)] 
    + a_weight.w * u_jointMatrix[int(a_joint.w)];
    
    vec4 pos = skinMatrix * vec4(vertexPosition, 1.0);

    // Transform vertex
    gl_Position =  MVP * pos;

    // World-space geometry 
    worldNormal = normalize(mat3(skinMatrix) * vertexNormal);
    worldPosition = pos.xyz;
}
