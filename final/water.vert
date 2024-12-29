#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 fragUV;
out vec3 worldPosition;
out vec4 fragPosLightSpace;

uniform mat4 MVP;
uniform sampler2D heightMap;
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;

void main() {
    fragUV = vertexUV;
    worldPosition = vertexPosition;

    // Displace based on height map
    float height = texture(heightMap, fragUV).r;
    height = sign(height) * (1.0 - exp(-abs(height)));
    vec3 displacedPosition = vertexPosition;
    displacedPosition.y += height * 0.4;  

    gl_Position = MVP * vec4(displacedPosition, 1.0);

    fragPosLightSpace = lightSpaceMatrix * vec4(worldPosition, 1.0);
}
