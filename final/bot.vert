#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 a_joint;
layout(location = 4) in vec4 a_weight;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 u_jointMatrix[25]; 

void main() {

    mat4 skinMatrix = 
    a_weight.x * u_jointMatrix[int(a_joint.x)] 
    + a_weight.y * u_jointMatrix[int(a_joint.y)] 
    + a_weight.z * u_jointMatrix[int(a_joint.z)] 
    + a_weight.w * u_jointMatrix[int(a_joint.w)];
    
    vec4 pos = skinMatrix * vec4(vertexPosition, 1.0);
    pos = pos * vec4(0.1, 0.1, 0.1, 1.0);
    pos += vec4(0.0f, -8.0, -62.0, 1.0f);

    // Transform vertex
    gl_Position =  MVP * pos;

    // World-space geometry 
    worldNormal = normalize(mat3(skinMatrix) * vertexNormal);
    worldPosition = pos.xyz;

    uv = vertexUV; 
}
